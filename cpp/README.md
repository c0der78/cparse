CParse
======
A C++ library to use the REST API at [parse.com](http://parse.com).


Setup
=====
- The build system uses [Premake](http://industriousone.com/premake) for cross platform compiling (OSX: brew install premake)
- run 'premake4 gmake' or 'premake4 vs2010' depending on your environment, which will build the makefile or project file
- run 'make' to compile the library and run the unit tests

Dependencies
============

- libcurl for the default client interface
- [libarg3](http://github.com/c0der78/arg3) for JSON
- [igloo](http://igloo-testing.org) for unit testing

Example
=======
```
Object obj("Example");

// add a string
obj.setString("name", "Harry Potter");

// add an int
obj.setInt("age", 24);

// add a double
obj.setDouble("money", 102.34);

if(obj.save())
	cout << "Success!" << endl;

/* Voila, we have saved an Example object on Parse.com */

if(obj.de1ete())
	cout <<  "Deleted!" << endl;

```

More Examples
=============

```
// setup your app info
Parse::set_application_id("AuIDhfjSJdiIFIFKDSJSJSY6KDKD8838");
Parse::set_api_key("LdfhJjdudJDjDJJMmUfkfjjhd7d7ld8484mJJJ");
```

```
// save in background
std::thread thread = someObj->saveInBackground([&](Object *obj) {
	cout << "Object Saved was " << obj->id() << endl;
});

thread.join();
```

```
// custom network api
class IOSClientInterface : public ClientInterface {
	....
	int request(http::method method, const string &url, map<string,string> headers, const string &data, string &response);
}

Parse::set_client_interface(new IOSClientInterface());
```

```
// references / pointers
Object a("Outer"), b("Inner");

a.setObject("inner", b);
```



