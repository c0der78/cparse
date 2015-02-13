#include <cparse/parse.h>
#include <cparse/object.h>
#include <cparse/exception.h>
#include <igloo/igloo.h>
#include <typeinfo>

using namespace cparse;
using namespace igloo;
using namespace std;

namespace cparse
{
    bool validate_class_name(const string &value);
}


class MySubclass : public Object
{
public:
    MySubclass() : Object("MySubclass")
    {

    }
};

Context(ObjectTest)
{
    Object *obj_;

    bool backgroundSuccess_;

    void SetUp()
    {
        obj_ = Object::create("TestCase");
    }

    void TearDown()
    {
        obj_->de1ete();
        delete obj_;
    }

    Spec(ValidClassName)
    {
        bool value = validate_class_name("_@#$@#$^*");

        Assert::That(false, Equals(value));

        value = validate_class_name("1234ABC");

        Assert::That(false, Equals(value));

        value = validate_class_name("ACBD123412");

        Assert::That(true, Equals(value));
    }

    Spec(get)
    {
        AssertThrows(cparse::Exception, obj_->get("testVal1"));

        JSON sValue("test string 1");

        obj_->set("testVal1", sValue);

        Assert::That(sValue, Equals(obj_->get("testVal1")));

        JSON iValue(1234567LL);

        obj_->set("testVal2", iValue);

        Assert::That(iValue, Equals(obj_->get("testVal2")));

        JSON bValue(true);

        obj_->set("testVal3", bValue);

        Assert::That(bValue, Equals(obj_->get("testVal3")));
    }

    Spec(save)
    {

        obj_->setInt("score", 1234);

        obj_->setString("name", "spok");

        Assert::That(obj_->save(), Equals(true));

        Assert::That(obj_->is_valid(), Equals(true));
    }

    Spec(saveInBackground)
    {
        obj_->setInt("score", 4567);

        obj_->setString("status", "saved in background");

        backgroundSuccess_ = false;

        std::thread thread = obj_->saveInBackground([&](Object * obj)
        {
            backgroundSuccess_ = true;
        });

        thread.join();

        Assert::That(backgroundSuccess_, Equals(true));
    }


    Spec(setInt)
    {
        obj_->setInt("id", 1234);

        Assert::That(obj_->getInt("id"), Equals(1234));
    }

    Spec(setString)
    {
        obj_->setString("id", "replaced");

        Assert::That(obj_->getString("id"), Equals("replaced"));
    }

    Spec(remove)
    {
        JSON value(1234);

        obj_->set("main", value);

        Assert::That(obj_->getString("main"), Equals("1234"));

        obj_->remove("main");

        Assert::That(obj_->contains("main"), Equals(false));

    }


    Spec(fetch)
    {
        MySubclass subclass;

        subclass.setInt("val1", 1234);

        Assert::That(subclass.save(), Equals(true));

        obj_->setObject("sub", subclass);

        Assert::That(obj_->save(), Equals(true));

        Object *fetched = Object::createWithoutData(obj_->className(), obj_->id());

        Assert::That(fetched->refresh(), Equals(true));

        Assert::That(fetched->isDataAvailable(), Equals(true));

        Assert::That(fetched->contains("sub"), Equals(true));

        Object *sub = fetched->getObject("sub");

        Assert::That(sub->isDataAvailable(), Equals(false));

        Assert::That(sub->contains("val1"), Equals(false));

        Assert::That(sub->fetch(), Equals(true));

        Assert::That(sub->isDataAvailable(), Equals(true));

        Assert::That(sub->getInt("val1"), Equals(1234));

        Assert::That(sub->de1ete(), Equals(true));

        delete fetched;
    }

    Spec(equality)
    {
        Object obj2("TestCase");

        Assert::That(*obj_ == obj2, Equals(false));

        Assert::That(*obj_ == *obj_, Equals(true));

        Assert::That(obj_->save(), Equals(true));

        Assert::That(*obj_ == obj2, Equals(false));

        Assert::That(obj2 == *obj_, Equals(false));

        Assert::That(obj_->toPointer() == *obj_, Equals(true));

        Assert::That(obj2.save(), Equals(true));

        Assert::That(*obj_ == *obj_, Equals(true));

        Assert::That(*obj_ == obj2, Equals(false));

        Assert::That(obj_->toPointer() == *obj_, Equals(true));

        Assert::That(obj2.de1ete(), Equals(true));
    }
};

Context(SubclassTest)
{
    Spec(className)
    {
        MySubclass subclass;

        Assert::That(subclass.className(), Equals("MySubclass"));
    }

    Spec(setInt)
    {
        MySubclass subclass;

        int value = rand() % 1000 + 1;

        subclass.setInt("val1", value);

        Assert::That(subclass.getInt("val1"), Equals(value));

        Assert::That(subclass.save(), Equals(true));

        Assert::That(subclass.de1ete(), Equals(true));
    }
};
