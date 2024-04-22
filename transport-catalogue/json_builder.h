#pragma once

#include "json.h"


namespace json
{
	class Builder
	{

		
		class KeyContext; //разрешен StartDict() и StartArray()
		class DictValueContext; // отсутствие метода Value()
		class ArrayValueContext;  // запрет Key() 
		class ValueContext; // обертка без ограничений для избежания конфликтов с типами
		// используется в DictValueContext и ArrayValueContext
		class StartDictContext;  // запрещен StartDict() и StartArray() 
	
	
	public:

		KeyContext Key(std::string key);
		

		ValueContext Value(Node::Value val);
		
		StartDictContext StartDict();
		
		ValueContext EndDict();
		
		ArrayValueContext StartArray();
	

		ValueContext EndArray();
		
		Node Build();
		

		~Builder() = default;
	

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
		std::vector<Node> node_stack_;
	private:

		class BaseContext {
		public:
			BaseContext(Builder& builder)
				: builder_(builder)
			{
			}
			KeyContext Key(std::string key);
		

			StartDictContext StartDict();
			
			ArrayValueContext StartArray();
			
			ValueContext EndDict();
			
			ValueContext EndArray();
			
			Node Build();
			



			Builder& builder_;
		};

		class KeyContext : public BaseContext {
		public:
			KeyContext(BaseContext base)
				: BaseContext(base)
			{
			}

			DictValueContext Value(Node::Value val);
			

			KeyContext Key(std::string key) = delete;
			ValueContext EndDict() = delete;
			ValueContext EndArray() = delete;
			Node Build() = delete;

		};

		class ValueContext : public BaseContext {
		public:
			ValueContext(BaseContext base)
				: BaseContext(base)
			{
			}

			ValueContext Value(Node::Value val);
			
		};

		class StartDictContext : public BaseContext {
		public:
			StartDictContext(BaseContext base)
				: BaseContext(base)
			{
			}


			ValueContext EndArray() = delete;
			Node Build() = delete;
			StartDictContext StartDict() = delete;
			ArrayValueContext StartArray() = delete;

		};

		

		

		

		class DictValueContext : public BaseContext {
		public:
			DictValueContext(BaseContext base)
				: BaseContext(base)
			{
			}

			ValueContext EndArray() = delete;
			Node Build() = delete;
			StartDictContext StartDict() = delete;
			ArrayValueContext StartArray() = delete;


		};

		class ArrayValueContext : public BaseContext {
		public:
			ArrayValueContext(BaseContext base)
				: BaseContext(base)
			{
			}
			ArrayValueContext Value(Node::Value val);
			

			KeyContext Key(std::string key) = delete;
			ValueContext EndDict() = delete;
			Node Build() = delete;


		};

	};



}