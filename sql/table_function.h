#ifndef TABLE_FUNCTION_INCLUDED
#define TABLE_FUNCTION_INCLUDED

/* Copyright (c) 2020, MariaDB Corporation. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1335  USA */


#include <json_lib.h>

class Json_table_nested_path : public Sql_alloc
{
public:
  bool m_null;
  json_path_t m_path;
  json_engine_t m_engine;
  json_path_t m_cur_path;

  Json_table_nested_path *m_parent;
  Json_table_nested_path *m_nested, *m_next_nested;
  Json_table_nested_path **m_nested_hook;
  Json_table_nested_path *m_cur_nested;
  Json_table_nested_path(Json_table_nested_path *parent_nest):
    m_parent(parent_nest), m_nested(0), m_next_nested(0),
    m_nested_hook(&m_nested) {}
  int set_path(THD *thd, const LEX_CSTRING &path);
  void scan_start(CHARSET_INFO *i_cs, const uchar *str, const uchar *end);
  int scan_next();
};


class Json_table_column : public Sql_alloc
{
public:
  enum enum_type
  {
    FOR_ORDINALITY,
    PATH,
    EXISTS_PATH
  };

  enum enum_on_type
  {
    ON_EMPTY,
    ON_ERROR
  };

  enum enum_on_response
  {
    RESPONSE_NOT_SPECIFIED,
    RESPONSE_ERROR,
    RESPONSE_NULL,
    RESPONSE_DEFAULT
  };

  struct On_response
  {
  public:
    Json_table_column::enum_on_response m_response;
    LEX_CSTRING m_default;
  };

  enum_type m_column_type;
  json_path_t m_path;
  On_response m_on_error;
  On_response m_on_empty;
  Create_field *m_field;
  Json_table_nested_path *m_nest;

  void set(enum_type ctype)
  {
    m_column_type= ctype;
  }
  int set(THD *thd, enum_type ctype, const LEX_CSTRING &path);
  Json_table_column(Create_field *f, Json_table_nested_path *nest) :
    m_field(f), m_nest(nest)
  {
    m_on_error.m_response= RESPONSE_NOT_SPECIFIED;
    m_on_empty.m_response= RESPONSE_NOT_SPECIFIED;
  }
};


class Table_function_json_table : public Sql_alloc
{
public:
  Item *m_json;
  Json_table_nested_path m_nested_path;
  List<Json_table_column> m_columns;
  table_map m_dep_tables;

  Table_function_json_table(Item *json): m_json(json), m_nested_path(0) {}

  int setup(THD *thd, TABLE_LIST *sql_table, COND **cond);
};


TABLE *create_table_for_function(THD *thd, TABLE_LIST *sql_table);

#endif /* TABLE_FUNCTION_INCLUDED */

