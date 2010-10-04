// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef WT_DBO_QUERY_IMPL_H_
#define WT_DBO_QUERY_IMPL_H_

#include <stdexcept>

#include <Wt/Dbo/Exception>
#include <Wt/Dbo/Field>
#include <Wt/Dbo/SqlStatement>

#include <Wt/Dbo/Field_impl.h>

#ifndef DOXYGEN_ONLY

namespace Wt {
  namespace Dbo {
    namespace Impl {

enum StatementKind { Select, Count };

extern std::string WTDBO_API
createQuerySql(StatementKind kind, const std::string& selectOption,
	       const std::vector<FieldInfo>& fields,
	       const std::string& from);

extern std::string WTDBO_API
createQuerySql(StatementKind kind, const std::string& selectOption,
	       const std::vector<FieldInfo>& fields,
	       const std::string& from,
	       const std::string& where,
	       const std::string& groupBy,
	       const std::string& orderBy,
	       int limit,
	       int offset);

extern void WTDBO_API
parseSql(const std::string& sql, std::string& selectOption,
	 std::vector<std::string>& aliases,
	 std::string& rest);

extern std::string WTDBO_API
selectColumns(const std::vector<FieldInfo>& fields);

template <class Result>
QueryBase<Result>::QueryBase()
  : session_(0)
{ }

template <class Result>
QueryBase<Result>::QueryBase(Session& session, const std::string& sql)
  : session_(&session)
{
  parseSql(sql, selectOption_, aliases_, from_);
}

template <class Result>
QueryBase<Result>::QueryBase(Session& session, const std::string& table,
			     const std::string& where)
  : session_(&session)
{
  from_ = "from \"" + table + "\" " + where;
}

template <class Result>
QueryBase<Result>& QueryBase<Result>::operator=(const QueryBase<Result>& other)
{
  session_ = other.session_;
  selectOption_ = other.selectOption_;
  from_ = other.from_;
  aliases_ = other.aliases_;

  return *this;
}

template <class Result>
std::vector<FieldInfo> QueryBase<Result>::fields() const
{
  std::vector<FieldInfo> result;

  if (aliases_.empty())
    query_result_traits<Result>::getFields(*session_, 0, result);
  else {
    std::vector<std::string> ls = aliases_;
    query_result_traits<Result>::getFields(*session_, &ls, result);
    if (!ls.empty())
      throw std::logic_error("Session::query(): too many aliases for result");
  }

  return result;
}

template <class Result>
Session& QueryBase<Result>::session() const
{
  return *session_;
}

template <class Result>
Result QueryBase<Result>::singleResult(const collection<Result>& results) const
{
  typename collection<Result>::const_iterator i = results.begin();
  if (i == results.end())
    return Result();
  else {
    Result result = *i;
    ++i;
    if (i != results.end())
      throw NoUniqueResultException();
    return result;
  }
}
    }

template <class Result>
Query<Result, DirectBinding>::Query()
  : statement_(0),
    countStatement_(0)
{ }

template <class Result>
Query<Result, DirectBinding>::Query(Session& session, const std::string& sql)
  : Impl::QueryBase<Result>(session, sql),
    statement_(0),
    countStatement_(0)
{
  prepareStatements();
}

template <class Result>
Query<Result, DirectBinding>::Query(Session& session, const std::string& table,
				    const std::string& where)
  : Impl::QueryBase<Result>(session, table, where),
    statement_(0),
    countStatement_(0)
{
  prepareStatements();
}

template <class Result>
Query<Result, DirectBinding>::~Query()
{
  if (statement_)
    statement_->done();
  if (countStatement_)
    countStatement_->done();
}

template <class Result>
template <typename T>
Query<Result, DirectBinding>&
Query<Result, DirectBinding>::bind(const T& value)
{
  sql_value_traits<T>::bind(value, this->statement_, column_, -1);
  sql_value_traits<T>::bind(value, this->countStatement_, column_, -1);

  ++column_;

  return *this;
}

template <class Result>
Result Query<Result, DirectBinding>::resultValue() const
{
  return this->singleResult(resultList());
}

template <class Result>
collection<Result> Query<Result, DirectBinding>::resultList() const
{
  if (!this->session_)
    return collection<Result>();

  if (!statement_)
    throw std::logic_error("Query<Result, DirectBinding>::resultList() "
			   "may be called only once");

  SqlStatement *s = this->statement_, *cs = this->countStatement_;
  this->statement_ = this->countStatement_ = 0;

  return collection<Result>(this->session_, s, cs);
}

template <class Result>
Query<Result, DirectBinding>::operator Result () const
{
  return resultValue();
}

template <class Result>
Query<Result, DirectBinding>::operator collection<Result> () const
{
  return resultList();
}

template <class Result>
void Query<Result, DirectBinding>::prepareStatements() const
{
  if (!this->session_)
    return;

  this->session_->flush();

  std::string sql;

  std::vector<FieldInfo> fs = this->fields();
  sql = Impl::createQuerySql(Impl::Select, this->selectOption_,
			     fs, this->from_);
  this->statement_ = this->session_->getOrPrepareStatement(sql);

  sql = Impl::createQuerySql(Impl::Count, this->selectOption_,
			     fs, this->from_);
  this->countStatement_ = this->session_->getOrPrepareStatement(sql);

  column_ = 0;
}

namespace Impl {
  template <typename T>
  void Parameter<T>::bind(SaveBaseAction& binder)
  {
    field(binder, v_, "parameter");
  }

  template <typename T>
  Parameter<T> *Parameter<T>::clone() const
  {
    return new Parameter<T>(v_);
  }
}

template <class Result>
Query<Result, DynamicBinding>::Query()
  : limit_(-1),
    offset_(-1)
{ }

template <class Result>
Query<Result, DynamicBinding>::Query(Session& session, const std::string& sql)
  : Impl::QueryBase<Result>(session, sql),
    limit_(-1),
    offset_(-1)
{ }

template <class Result>
Query<Result, DynamicBinding>::Query(Session& session,
				     const std::string& table,
				     const std::string& where)
  : Impl::QueryBase<Result>(session, table, where),
    limit_(-1),
    offset_(-1)
{ }

template <class Result>
Query<Result, DynamicBinding>
::Query(const Query<Result, DynamicBinding>& other)
  : Impl::QueryBase<Result>(other),
    where_(other.where_),
    groupBy_(other.groupBy_),
    orderBy_(other.orderBy_),
    limit_(-1),
    offset_(-1)
{ 
  for (unsigned i = 0; i < other.parameters_.size(); ++i)
    parameters_.push_back(other.parameters_[i]->clone());
}

template <class Result>
Query<Result, DynamicBinding>&
Query<Result, DynamicBinding>::operator=
(const Query<Result, DynamicBinding>& other)
{
  Impl::QueryBase<Result>::operator=(other);
  where_ = other.where_;
  groupBy_ = other.groupBy_;
  orderBy_ = other.orderBy_;
  limit_ = other.limit_;
  offset_ = other.offset_;

  for (unsigned i = 0; i < other.parameters_.size(); ++i)
    parameters_.push_back(other.parameters_[i]->clone());

  return *this;
}

template <class Result>
Query<Result, DynamicBinding>::~Query()
{
  reset();
}

template <class Result>
template <typename T>
Query<Result, DynamicBinding>&
Query<Result, DynamicBinding>::bind(const T& value)
{
  parameters_.push_back(new Impl::Parameter<T>(value));

  return *this;
}

template <class Result>
Query<Result, DynamicBinding>&
Query<Result, DynamicBinding>::where(const std::string& where)
{
  if (!where_.empty())
    where_ += " and ";

  where_ += "(" + where + ")";

  return *this;
}

template <class Result>
Query<Result, DynamicBinding>&
Query<Result, DynamicBinding>::orderBy(const std::string& orderBy)
{
  orderBy_ = orderBy;

  return *this;
}

template <class Result>
Query<Result, DynamicBinding>&
Query<Result, DynamicBinding>::groupBy(const std::string& groupBy)
{
  groupBy_ = groupBy;

  return *this;
}

template <class Result>
Query<Result, DynamicBinding>&
Query<Result, DynamicBinding>::offset(int offset)
{
  offset_ = offset;

  return *this;
}

template <class Result>
Query<Result, DynamicBinding>&
Query<Result, DynamicBinding>::limit(int limit)
{
  limit_ = limit;

  return *this;
}

template <class Result>
Result Query<Result, DynamicBinding>::resultValue() const
{
  return this->singleResult(resultList());
}

template <class Result>
collection<Result> Query<Result, DynamicBinding>::resultList() const
{
  if (!this->session_)
    return collection<Result>();

  this->session_->flush();

  std::vector<FieldInfo> fs = this->fields();

  std::string sql;
  sql = Impl::createQuerySql(Impl::Select, this->selectOption_, fs, this->from_,
			     where_, groupBy_, orderBy_, limit_, offset_);
  SqlStatement *statement = this->session_->getOrPrepareStatement(sql);
  bindParameters(statement);

  sql = Impl::createQuerySql(Impl::Count, this->selectOption_, fs, this->from_,
			     where_, groupBy_, orderBy_, limit_, offset_);
  SqlStatement *countStatement = this->session_->getOrPrepareStatement(sql);
 
  bindParameters(countStatement);

  return collection<Result>(this->session_, statement, countStatement);
}

template <class Result>
Query<Result, DynamicBinding>::operator Result () const
{
  return resultValue();
}

template <class Result>
Query<Result, DynamicBinding>::operator collection<Result> () const
{
  return resultList();
}

template <class Result>
void Query<Result, DynamicBinding>::bindParameters(SqlStatement *statement)
  const
{
  SaveBaseAction binder(statement, 0);

  for (unsigned i = 0; i < parameters_.size(); ++i)
    parameters_[i]->bind(binder);

  if (limit_ != -1) {
    int v = limit_;
    field(binder, v, "limit");
  }

  if (offset_ != -1) {
    int v = offset_;
    field(binder, v, "offset");
  }
}

template <class Result>
void Query<Result, DynamicBinding>::reset()
{
  for (unsigned i = 0; i < parameters_.size(); ++i)
    delete parameters_[i];

  parameters_.clear();
}

  }
}

#endif // DOXYGEN_ONLY

#endif // WT_DBO_QUERY_IMPL_H_
