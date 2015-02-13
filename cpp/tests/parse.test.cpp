#include <cparse/object.h>
#include <cparse/parse.h>
#include <igloo/igloo.h>
#include <fstream>

using namespace cparse;
using namespace igloo;
using namespace std;

void die(const char *message)
{
    printf("ERROR: %s\n", message);
    abort();
}

int main(void)
{
    ifstream file("./parse.test.json");
	
    if (!file.is_open())
    {
	file.open("./tests/parse.test.json");
	if(!file.is_open())
           die("parse.test.json not found");
    }
    arg3::json::object config(file);

    if (config.contains("parseAppId"))
        Parse::set_application_id(config.get_string("parseAppId"));
    else
        die("No app id");

    if (config.contains("parseApiKey"))
        Parse::set_api_key(config.get_string("parseApiKey"));
    else
        die("No api key");

    return TestRunner::RunAllTests();
}
