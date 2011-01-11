/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _LIBLDR_MODEL_H_
#define _LIBLDR_MODEL_H_

#include <string>

#include <list>
#include <map>
#include <set>
#include <stack>
#include <utility>
#include <vector>

#include "common.h"
#include "elements.h"
#include "extension.h"

namespace ldraw
{

class extension;
class model_multipart;
class part_library;
class reader;

// Represents a single-part model.
class LIBLDR_EXPORT model
{
  public:
	enum model_type { primitive, part, submodel, external_file, general };
	typedef std::vector<element_base*>::const_iterator const_iterator;
	typedef std::vector<element_base*>::const_reverse_iterator reverse_iterator;
	
	explicit model(model_multipart *parent = 0L)
		: m_null(true), m_parent(parent), m_model_type(general) {}
	model(const std::string &desc, const std::string &name, const std::string &author, model_multipart *parent = 0L);
	~model();
	
	bool is_null() const { return m_null; }
	
	bool is_submodel_of(const model_multipart *m) const;
	
	model_type modeltype() const { return m_model_type; }
	const std::string& desc() const { return m_desc; }
	const std::string& name() const { return m_name; }
	const std::string& author() const { return m_author; }
	std::list<std::string> header(const std::string &key) const;
	const std::multimap<std::string, std::string> headers() const { return m_headers; }
	
	model_multipart* parent() { return m_parent; }
	const model_multipart* parent() const { return m_parent; }
	const std::vector<element_base*>& elements() const { return m_elements; }

	// Edit
	int size() const;
	element_base* at(unsigned int index);
	element_base* operator[] (unsigned int index);
	void insert_element(element_base *e, int pos = -1);
	bool delete_element(int pos = -1);
	
	void set_modeltype(model_type t) { m_model_type = t; }
	void set_desc(const std::string &desc) { m_desc = desc; }
	void set_name(const std::string &name) { m_name = name; }
	void set_author(const std::string &author) { m_author = author; }
	void set_header(const std::string &key, const std::string &value);
	void remove_header(const std::string &key);

	template <class T> T* init_custom_data(void *data = 0L, bool preserve = false)
	{
		if (custom_data<T>()) {
			if (preserve)
				return 0L;
			delete m_data[T::identifier()];
		}
		
		T *ndata = new T(this, data);
		m_data[T::identifier()] = ndata;

		return ndata;
	}
	
	template <class T> T* custom_data() const
	{
		std::map<std::string, extension *>::const_iterator it = m_data.find(T::identifier());
		
		if (it == m_data.end())
			return 0L;
	
		return dynamic_cast<T *>((*it).second);
	}

	template <class T> const T* const_custom_data() const
	{
		return custom_data<T>();
	}
	
	template <class T> void update_custom_data(void *data = 0L, bool recursive = false, bool preserve = false)
	{
		if (recursive) {
			for (iterator it = m_elements.begin(); it != m_elements.end(); ++it) {
				if ((*it)->get_type() == type_ref) {
					model *m = CAST_AS_REF(*it)->get_model();
					if (!m->custom_data<T>())
						m->init_custom_data<T>(data, preserve);
					m->update_custom_data<T>(data, recursive, preserve);
				}
			}
			
		}

		if (!custom_data<T>())
			init_custom_data<T>(data, preserve);
		else
			m_data[T::identifier()]->set_data(data);
			
		m_data[T::identifier()]->update();
	}
	
	template <class T> void delete_custom_data()
	{
		std::map<std::string, extension *>::iterator it = m_data.find(T::identifier());
		if (it != m_data.end()) {
			delete *it;
			m_data.erase(it);
		}
	}		
	
	void clear();
	
  private:
	typedef std::vector<element_base*>::iterator iterator;
	
	friend class model_multipart;
	friend class part_library;
	friend class reader;

	void set_parent(model_multipart *parent) { m_parent = parent; }
	
	std::string m_desc;
	std::string m_name;
	std::string m_author;
	
	std::vector<element_base*> m_elements;

	std::multimap<std::string, std::string> m_headers;
	
	bool m_null;
	model_multipart *m_parent;

	std::map<std::string, extension *> m_data;
	
	model_type m_model_type;
};

// Multi-part model.
class LIBLDR_EXPORT model_multipart
{
  public:
	typedef std::map<std::string, model*>::iterator submodel_iterator;
	typedef std::map<std::string, model*>::const_iterator submodel_const_iterator;
	typedef std::map<std::string, model*>::reverse_iterator submodel_reverse_iterator;
	
	model_multipart() { m_main_model.set_parent(this); }
	~model_multipart() { clear(); }
	
	int count() const { return m_submodel_list.size(); }
	
	model* main_model() { return &m_main_model; }
	const model* main_model() const { return &m_main_model; }
	
	std::map<std::string, model*>& submodel_list() { return m_submodel_list; }
	const std::map<std::string, model*>& submodel_list() const { return m_submodel_list; }
	
	bool contains(const model *m) const;

	void link_submodels();
	void link_submodel(model *m);
	bool link_submodel_element(element_ref *r);
	
	model* find_submodel(const std::string &name);
	bool insert_submodel(model *m);
	bool insert_submodel(model *m, const std::string &key);
	bool remove_submodel(const std::string &name);
	bool rename_submodel(const std::string &name, const std::string &newname);
	
	void clear();
	
	//void operator=(const model_multipart &rhs);

  private:
	model_multipart* find_external_model(const std::string &name);
	model_multipart* load_external_model(const reader &r, const std::string &name);
	bool remove_external_model(const std::string &name);
	
  private:
	model m_main_model;
	std::map<std::string, model*> m_submodel_list;
	std::map<std::string, model_multipart*> m_external_model_list;
};

}

#endif
