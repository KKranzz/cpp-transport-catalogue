#pragma once

#include "json.h"


namespace json
{
	class Builder
	{

		class BuilderContext;
		class KeyContext;
		class DictValueContext; //
		class ArrayValueContext; // 
		class ValueContext;
		class StartDictContext;
		class StartArrayContext;
		class EndDictContext;
		class EndArrayContext;
	public:

		KeyContext Key(std::string key)
		{
			if (!dict_active && key_active)
			{
				throw std::logic_error("Key called without a dictionary started");
			}
			key_ = std::move(key);
			key_active = true;
			return KeyContext(*this);
		}

		ValueContext Value(Node::Value val)
		{

			if (std::holds_alternative<nullptr_t>(val))
			{
				if (key_active && dict_active)
				{
					dict_.insert({ key_,  std::get<nullptr_t>(val) });
					key_active = false;
				}
				else if (array_active && !key_active && !dict_active)
				{
					arr_.push_back(std::get<nullptr_t>(val));
				}
				else if (first_elem)
				{
					first_ = std::get<nullptr_t>(val);

					first_elem = false;
				}
				else
				{
					throw std::logic_error("Value sequence has broken");
				}
			}
			if (std::holds_alternative<Array>(val))
			{
				if (key_active && dict_active)
				{
					dict_.insert({ key_,  std::get<Array>(val) });
					key_active = false;
				}
				else if (array_active && !key_active && !dict_active)
				{
					arr_.push_back(std::get<Array>(val));
				}
				else if (first_elem)
				{
					first_ = std::get<Array>(val);

					first_elem = false;
				}
				else
				{
					throw std::logic_error("Value sequence has broken");
				}
			}
			if (std::holds_alternative<Dict>(val))
			{
				if (key_active && dict_active)
				{
					dict_.insert({ key_,  std::get<Dict>(val) });
					key_active = false;
				}
				else if (array_active && !key_active && !dict_active)
				{
					arr_.push_back(std::get<Dict>(val));
				}
				else if (first_elem)
				{
					first_ = std::get<Dict>(val);

					first_elem = false;
				}
				else
				{
					throw std::logic_error("Value sequence has broken");
				}

			}
			if (std::holds_alternative<bool>(val))
			{
				if (key_active && dict_active)
				{
					dict_.insert({ key_,  std::get<bool>(val) });
					key_active = false;
				}
				else if (array_active && !key_active && !dict_active)
				{
					arr_.push_back(std::get<bool>(val));
				}
				else if (first_elem)
				{
					first_ = std::get<bool>(val);

					first_elem = false;
				}
				else
				{
					throw std::logic_error("Value sequence has broken");
				}
			}
			if (std::holds_alternative<double>(val))
			{
				if (key_active && dict_active)
				{
					dict_.insert({ key_,  std::get<double>(val) });
					key_active = false;
				}
				else if (array_active && !key_active && !dict_active)
				{
					arr_.push_back(std::get<double>(val));
				}
				else if (first_elem)
				{
					first_ = std::get<double>(val);

					first_elem = false;
				}
				else
				{
					throw std::logic_error("Value sequence has broken");
				}
			}
			if (std::holds_alternative<int>(val))
			{
				if (key_active && dict_active)
				{
					dict_.insert({ key_,  std::get<int>(val) });
					key_active = false;
				}
				else if (array_active && !key_active && !dict_active)
				{
					arr_.push_back(std::get<int>(val));
				}
				else if (first_elem)
				{
					first_ = std::get<int>(val);

					first_elem = false;
				}
				else
				{
					throw std::logic_error("Value sequence has broken");
				}
			}

			if (std::holds_alternative<std::string>(val))
			{
				if (key_active && dict_active)
				{
					dict_.insert({ key_,  std::get<std::string>(val) });
					key_active = false;
				}
				else if (array_active && !key_active && !dict_active)
				{
					arr_.push_back(std::get<std::string>(val));
				}
				else if (first_elem)
				{
					first_ = std::get<std::string>(val);

					first_elem = false;
				}
				else
				{
					throw std::logic_error("Value sequence has broken");
				}
			}




			return ValueContext(*this);
		}
		StartDictContext StartDict()
		{
			if (dict_active)
			{
				if (!key_active)
				{
					throw std::logic_error("Wrong StartDict() Flag(without key in dict added)");
				}
				key_active = false;
				keys_.push_back(std::move(key_));
				node_stack_.push_back(new Node(std::move(dict_)));
				dict_.clear();
			}
			else if (array_active)
			{
				node_stack_.push_back(new Node(std::move(arr_)));
				arr_.clear();
				array_active = false;
			}
			else if (first_elem)
			{
				first_elem = false;
			}
			else { throw std::logic_error("Wrong StartDict() Flag"); }
			dict_active = true;

			return StartDictContext(*this);
		}

		EndDictContext EndDict()
		{
			if (first_elem || array_active || key_active || !dict_active)
			{
				throw std::logic_error("Incorrect calling EndDict()");
			}



			if (node_stack_.empty())
			{
				dict_active = false;
				first_ = Node(std::move(dict_));
			}
			else if (node_stack_.back()->IsArray())
			{
				dict_active = false;
				array_active = true;
				arr_ = node_stack_.back()->AsArray();
				delete node_stack_.back();
				node_stack_.pop_back();
				arr_.push_back(std::move(dict_));

			}
			else if (node_stack_.back()->IsMap())
			{
				Dict bf = std::move(dict_);
				dict_ = node_stack_.back()->AsMap();
				delete node_stack_.back();
				node_stack_.pop_back();
				key_ = keys_.back();
				keys_.pop_back();
				dict_.insert({ key_, std::move(bf) });

			}


			if (!dict_active)
			{
				dict_.clear();
			}
			return EndDictContext(*this);
		}

		StartArrayContext StartArray()
		{
			if (dict_active)
			{
				if (!key_active)
				{
					throw std::logic_error("Wrong StartArray() Flag(without key in dict added)");
				}
				key_active = false;
				keys_.push_back(std::move(key_));
				node_stack_.push_back(new Node(std::move(dict_)));
				dict_.clear();
				dict_active = false;
			}
			else if (array_active)
			{
				node_stack_.push_back(new Node(std::move(arr_)));
				arr_.clear();

			}
			else if (first_elem)
			{
				first_elem = false;
			}
			else { throw std::logic_error("Wrong StartArray() Flag"); }
			array_active = true;

			return StartArrayContext(*this);
		}

		EndArrayContext EndArray()
		{
			if (first_elem || dict_active || key_active || !array_active)
			{
				throw std::logic_error("Incorrect calling EndArray()");
			}



			if (node_stack_.empty())
			{
				array_active = false;
				first_ = Node(std::move(arr_));
			}
			else if (node_stack_.back()->IsArray())
			{

				Array bf = std::move(arr_);
				arr_ = node_stack_.back()->AsArray();
				delete node_stack_.back();
				node_stack_.pop_back();
				arr_.push_back(std::move(bf));

			}
			else if (node_stack_.back()->IsMap())
			{
				array_active = false;
				dict_active = true;
				dict_ = node_stack_.back()->AsMap();
				delete node_stack_.back();
				node_stack_.pop_back();
				key_ = keys_.back();
				keys_.pop_back();
				dict_.insert({ key_, std::move(arr_) });

			}


			if (!array_active) { arr_.clear(); }

			return EndArrayContext(*this);
		}

		Node Build()
		{
			if (key_active || dict_active || array_active || first_elem)
			{
				throw std::logic_error("Incorrect build");
			}

			/*	if (!arr_.empty())
				{
					return Node(std::move(arr_));
				}
				else if (!dict_.empty())
				{
					return Node(std::move(dict_));
				}
				*/

			return Node(std::move(first_));

		}


		~Builder()
		{
			for (auto& node : node_stack_)
			{
				delete node;
			}


		}

		//flags
		bool key_active = false;
		bool array_active = false;
		bool dict_active = false;
		bool first_elem = true;
		//meta
		std::string key_;
		std::vector<std::string> keys_;
		Array arr_;
		Dict dict_;
		Node first_;
		//data

		Node root_;
		std::vector<Node*> node_stack_;
	private:

		class BaseContext {
		public:
			BaseContext(Builder& builder)
				: builder_(builder)
			{
			}
			KeyContext Key(std::string key)
			{
				return builder_.Key(std::move(key));
			}

			StartDictContext StartDict()
			{
				return builder_.StartDict();
			}
			StartArrayContext StartArray()
			{
				return builder_.StartArray();
			}
			EndDictContext EndDict()
			{
				return builder_.EndDict();
			}
			EndArrayContext EndArray()
			{
				return builder_.EndArray();
			}
			Node Build()
			{
				return builder_.Build();
			}



			Builder& builder_;
		};

		class KeyContext : public BaseContext {
		public:
			KeyContext(BaseContext base)
				: BaseContext(base)
			{
			}

			DictValueContext Value(Node::Value val)
			{
				auto default_value = ValueContext(builder_);
				default_value.Value(val);
				return DictValueContext(default_value.builder_);
			}

			KeyContext Key(std::string key) = delete;
			EndDictContext EndDict() = delete;
			EndArrayContext EndArray() = delete;
			Node Build() = delete;


			/*
			KeyContext Key(std::string key) = delete;
			EndDictContext EndDict() = delete;
			EndArrayContext EndArray() = delete;
			Node Build() = delete;
			StartDictContext StartDict() = delete;
			StartArrayContext StartArray() = delete;

			*/
		};

		class ValueContext : public BaseContext {
		public:
			ValueContext(BaseContext base)
				: BaseContext(base)
			{
			}

			ValueContext Value(Node::Value val)
			{
				return builder_.Value(val);
			}
		};

		class StartDictContext : public BaseContext {
		public:
			StartDictContext(BaseContext base)
				: BaseContext(base)
			{
			}


			EndArrayContext EndArray() = delete;
			Node Build() = delete;
			StartDictContext StartDict() = delete;
			StartArrayContext StartArray() = delete;

		};

		class StartArrayContext : public BaseContext {
		public:
			StartArrayContext(BaseContext base)
				: BaseContext(base)
			{
			}

			ArrayValueContext Value(Node::Value val)
			{
				ValueContext default_(builder_);
				default_.Value(val);
				return ArrayValueContext(default_.builder_);
			}
			KeyContext Key(std::string key) = delete;
			EndDictContext EndDict() = delete;

			Node Build() = delete;

		};

		class EndDictContext : public BaseContext {
		public:
			EndDictContext(BaseContext base)
				: BaseContext(base)
			{
			}
			ValueContext Value(Node::Value val)
			{
				ValueContext default_(builder_);
				return default_.Value(val);
			}
		};

		class EndArrayContext : public BaseContext {
		public:
			EndArrayContext(BaseContext base)
				: BaseContext(base)
			{
			}
			ValueContext Value(Node::Value val)
			{
				ValueContext default_(builder_);
				return default_.Value(val);
			}
		};

		class DictValueContext : public BaseContext {
		public:
			DictValueContext(BaseContext base)
				: BaseContext(base)
			{
			}

			EndArrayContext EndArray() = delete;
			Node Build() = delete;
			StartDictContext StartDict() = delete;
			StartArrayContext StartArray() = delete;


		};


		class ArrayValueContext : public BaseContext {
		public:
			ArrayValueContext(BaseContext base)
				: BaseContext(base)
			{
			}
			ArrayValueContext Value(Node::Value val)
			{
				ValueContext default_(builder_);
				default_.Value(val);
				return ArrayValueContext(default_.builder_);
			}

			KeyContext Key(std::string key) = delete;
			EndDictContext EndDict() = delete;
			Node Build() = delete;


		};

	};



}