#include <tuple>
#include <cassert>
#include <iostream>
#include <typeinfo>

template < class Type, int Size >
struct Field
{
  typedef Type type;
  static constexpr int size = Size;

  Field() = default;
  Field(type value) : value_(value) {}
  type value_ = type();
};

template < size_t T >
struct visitor
{
  template < class Gettable, class Function >
  static void visit(const Gettable& g, const Function& f)
  {
    visitor< T - 1 >::visit(g, f);
    f(std::get< T - 1 >(g));
  }
};

template<>
struct visitor<0>
{
  template < class Gettable, class Function >
  static void visit(const Gettable& g, const Function& f) {}
};

template < typename... Fields>
struct message
{
  static constexpr int size = sizeof...(Fields);
  typedef std::tuple< Fields... > fields_t;

  message(fields_t f) : fields(f) {}

  fields_t fields;

  void print()
  {
    visitor< sizeof...(Fields) >::visit(fields, [](auto f){ std::cout << f.value_ << std::endl; });
  }
};

template < class Type, int Size >
struct parser
{
  static Field< Type, Size > parse(char *source) { assert(!"Not implemented"); }
  // overloads for other sources are possible
};

template < int Size >
struct parser< int, Size >
{
  static Field< int, Size > parse(char *source)
  {
    std::cout << "parser< int, " << Size << " >::parse" << std::endl;
    return Field< int, Size >(Size);
  }
};

template < int Size >
struct parser< char, Size >
{
  static Field< char, Size > parse(char *source)
  {
    std::cout << "parser< char, " << Size << " >::parse" << std::endl;
    return Field< char, Size >(Size);
  }
};

template < typename Field >
std::tuple< Field >  parse(std::tuple< Field >, char *source)
{
  std::cout << "std::tuple< Field > parse(char * source)" << std::endl;
  return std::make_tuple(parser< typename Field::type, Field::size >::parse(0));
}

template < typename Field, typename Next, typename... Fields >
std::tuple< Field, Next, Fields...>  parse(std::tuple< Field, Next, Fields... >, char *source)
{
  std::cout << "std::tuple< "
            << typeid(Field).name() << ", "
            << typeid(Next).name() << ", Fields...>  parse(char * source)" << std::endl;
  return std::tuple_cat(
    parse(std::tuple< Field>(), source),
    parse(std::tuple< Next, Fields... >(), source)
  );
}

template < typename Message >
Message parseMessage()
{
  return Message( parse(typename Message::fields_t(), 0) );
}

typedef message< Field< int, 1 >, Field< int, 2 > > message1;
typedef message< Field< int, 3 >, Field< int, 4 >, Field< char, 68 > > message2;

int main()
{

  auto f2 = parseMessage< message2 >();

  f2.print();
}
