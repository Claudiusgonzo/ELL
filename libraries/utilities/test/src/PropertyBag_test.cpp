
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PropertyBag_test.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PropertyBag_test.h"

#include <utilities/include/JsonArchiver.h>
#include <utilities/include/PropertyBag.h>

#include <testing/include/testing.h>

#include <iostream>
#include <string>
#include <vector>

namespace ell
{
using namespace utilities;

void TestPropertyBag()
{
    PropertyBag metadata;

    // IsEmpty test
    testing::ProcessTest("PropertyBag::IsEmpty", testing::IsTrue(metadata.IsEmpty()));
    testing::ProcessTest("PropertyBag::HasEntry", testing::IsFalse(metadata.HasEntry("a")));

    metadata.SetEntry("a", std::string("1"));
    testing::ProcessTest("PropertyBag::SetEntry/HasEntry", testing::IsTrue(metadata.HasEntry("a")));
    testing::ProcessTest("PropertyBag::SetEntry/GetEntries", testing::IsEqual(metadata.GetEntry<std::string>("a"), "1"));

    metadata.SetEntry("a", std::string("2"));
    testing::ProcessTest("PropertyBag::SetEntry/GetEntries", testing::IsEqual(metadata.GetEntry<std::string>("a"), "2"));

    auto foo = metadata.GetEntry<std::string>("a");
    testing::ProcessTest("PropertyBag::SetEntry/GetEntries foo", testing::IsEqual(foo, "2"));

    auto foo2 = metadata.GetEntry("a");
    testing::ProcessTest("PropertyBag::SetEntry/GetEntries foo2", testing::IsTrue(foo2.IsType<std::string>() && foo2.GetValue<std::string>() == "2"));

    auto removedEntry = metadata.RemoveEntry("a");
    testing::ProcessTest("PropertyBag::RemoveEntry", testing::IsEqual(removedEntry.GetValue<std::string>(), "2"));

    auto removedEntries = metadata.RemoveEntry("a");
    testing::ProcessTest("PropertyBag::IsEmpty", testing::IsTrue(metadata.IsEmpty()));

    metadata.SetEntry("a", 1);
    metadata["a"]++;
    metadata["a"]++;
    metadata["a"]++;
    testing::ProcessTest("PropertyBag::operator[] (non-const)", testing::IsEqual(metadata.GetEntry<int>("a"), 4));

    //
    // Serialization tests
    //
    metadata.Clear();
    metadata["a"] = std::string("foo");
    metadata["b"] = std::vector<std::string>{ "hello", "world" };
    metadata["c"] = 4;
    metadata["d"] = std::vector<int>{ 5, 6, 7, 8 };
    metadata["e"] = 5.0;

    // this has a side effect of adding "f" with void value, make sure the property bag can still serialize after this...
    testing::ProcessTest("PropertyBag property access has side effect", metadata["f"].IsEmpty());

    std::stringstream strstream;
    {
        JsonArchiver archiver(strstream);
        archiver << metadata;
    }

    std::cout << "Archived property bag:" << std::endl;
    std::cout << strstream.str() << std::endl;

    SerializationContext context;
    JsonUnarchiver unarchiver(strstream, context);
    PropertyBag metadata2;
    unarchiver >> metadata2;

    testing::ProcessTest("Deserialize PropertyBag string", testing::IsEqual(metadata2.GetEntry<std::string>("a"), "foo"));
    testing::ProcessTest("Deserialize PropertyBag vector<string>", testing::IsEqual(metadata2.GetEntry<std::vector<std::string>>("b"), { "hello", "world" }));
    testing::ProcessTest("Deserialize PropertyBag int", testing::IsEqual(metadata2.GetEntry<int>("c"), 4));
    testing::ProcessTest("Deserialize PropertyBag vector<int>", testing::IsEqual(metadata2.GetEntry<std::vector<int>>("d"), { 5, 6, 7, 8 }));
    testing::ProcessTest("Deserialize PropertyBag double", testing::IsEqual(metadata2.GetEntry<double>("e"), 5.0));

    SerializationContext context2;
    std::cout << strstream.str() << std::endl;
    std::stringstream strstream2(strstream.str());
    JsonUnarchiver unarchiver2(strstream2, context2);
    PropertyBag metadata3;
    unarchiver2 >> metadata3;
    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata3.GetEntry<std::string>("a"), "foo"));
    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata3.GetEntry<std::vector<std::string>>("b"), { "hello", "world" }));
    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata3.GetEntry<int>("c"), 4));
    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata3.GetEntry<std::vector<int>>("d"), { 5, 6, 7, 8 }));
    testing::ProcessTest("Deserialize PropertyBag", testing::IsEqual(metadata3.GetEntry<double>("e"), 5.0));
}

void TestRecursivePropertyBag()
{
    PropertyBag inner;
    PropertyBag outer;

    //
    // Serialization tests
    //
    inner.Clear();
    inner["a"] = std::string("foo");
    inner["b"] = std::vector<std::string>{ "hello", "world" };
    inner["c"] = 4;
    inner["d"] = std::vector<int>{ 5, 6, 7, 8 };
    inner["e"] = 5.0;

    outer["inner"] = inner;

    std::stringstream strstream;
    {
        JsonArchiver archiver(strstream);
        archiver << outer;
    }

    std::cout << "Archived recursive property bag:" << std::endl;
    std::cout << strstream.str() << std::endl;

    SerializationContext context;
    JsonUnarchiver unarchiver(strstream, context);
    PropertyBag outer2;
    unarchiver >> outer2;
}
} // namespace ell
