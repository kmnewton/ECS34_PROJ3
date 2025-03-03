#include <gtest/gtest.h>
#include "StringUtils.h"

TEST(StringUtilsTest, SliceTest){
    EXPECT_EQ("me", StringUtils::Slice("meow", 0, 2));
    EXPECT_EQ("eo", StringUtils::Slice("meow", -3, -1));
}

TEST(StringUtilsTest, Capitalize){
    EXPECT_EQ("Meow", StringUtils::Capitalize("meow"));
    EXPECT_EQ("Meow", StringUtils::Capitalize("MEOW"));
    EXPECT_EQ(" meow", StringUtils::Capitalize(" meow"));
}

TEST(StringUtilsTest, Upper){
    EXPECT_EQ("MEOW", StringUtils::Upper("meow"));
}

TEST(StringUtilsTest, Lower){
    EXPECT_EQ("meow", StringUtils::Lower("MEOW"));
}

TEST(StringUtilsTest, LStrip){
    EXPECT_EQ("meow", StringUtils::LStrip("   meow"));
}

TEST(StringUtilsTest, RStrip){
    EXPECT_EQ("me ow", StringUtils::RStrip("me ow     "));
}

TEST(StringUtilsTest, Strip){
    EXPECT_EQ("meow", StringUtils::Strip("       meow     "));
}

TEST(StringUtilsTest, Center){
    EXPECT_EQ("   meow   ", StringUtils::Center("meow", 10));
}

TEST(StringUtilsTest, LJust){
    EXPECT_EQ("meow      ", StringUtils::LJust("meow", 10));
}

TEST(StringUtilsTest, RJust){
    EXPECT_EQ("      meow", StringUtils::RJust("meow", 10));
}

TEST(StringUtilsTest, Replace){
    EXPECT_EQ("meowmeowmeowmeowmeow", StringUtils::Replace("meowlalameowlalameow", "lala", "meow"));
}

TEST(StringUtilsTest, Split){
    // EXPECT_EQ({"meow", "meow", "meow"}, StringUtils::Split("meow/meow/meow", "/"));
}

TEST(StringUtilsTest, Join){
    EXPECT_EQ("meow meow", StringUtils::Join(" ", {"meow", "meow"}));
}

TEST(StringUtilsTest, ExpandTabs){
    
}

TEST(StringUtilsTest, EditDistance){
    
}
