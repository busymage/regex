#include <gtest/gtest.h>
#include <Engine/Engine.hpp>
#include <vector>

TEST(EngineTest, empty)
{
    Engine e("");
    ASSERT_TRUE(e.accept(""));
    ASSERT_FALSE(e.accept(" "));
}

TEST(EngineTest, accpetZeroOrMoreCharacter)
{
    Engine e("a*");
    ASSERT_TRUE(e.accept(""));
    ASSERT_TRUE(e.accept("a"));
    ASSERT_TRUE(e.accept("aaa"));
}

TEST(EngineTest, accpetOneOrMoreCharacter)
{
    {
        Engine e("a+");
        ASSERT_FALSE(e.accept(""));
        ASSERT_FALSE(e.accept("a+"));
        ASSERT_TRUE(e.accept("a"));
        ASSERT_TRUE(e.accept("aaa"));
    }
    {
        Engine e("a|b+");
        ASSERT_FALSE(e.accept(""));
        ASSERT_FALSE(e.accept("a+"));
        ASSERT_TRUE(e.accept("a"));
        ASSERT_TRUE(e.accept("b"));
        ASSERT_TRUE(e.accept("bb"));
        ASSERT_FALSE(e.accept("aaa"));
        ASSERT_FALSE(e.accept("ab"));
        ASSERT_FALSE(e.accept("abb"));
    }
}

TEST(EngineTest, accpetZeroOrOneCharacter)
{
    {
        Engine e("a?");
        ASSERT_TRUE(e.accept(""));
        ASSERT_FALSE(e.accept("a?"));
        ASSERT_TRUE(e.accept("a"));
        ASSERT_FALSE(e.accept("aaa"));
        ASSERT_FALSE(e.accept("?"));
    }

    {
        Engine e("(ab)?");
        ASSERT_TRUE(e.accept(""));
        ASSERT_FALSE(e.accept("a"));
        ASSERT_TRUE(e.accept("ab"));
        ASSERT_FALSE(e.accept("b"));
        ASSERT_FALSE(e.accept("abab"));
    }
}

TEST(EngineTest, accpet)
{
    Engine e("(a|b)*abb(a|b)*");
    ASSERT_TRUE(e.accept("abb"));
    ASSERT_TRUE(e.accept("abaabbbb"));
    ASSERT_TRUE(e.accept("bbaabbaba"));
    
    ASSERT_FALSE(e.accept("ab"));
    ASSERT_FALSE(e.accept("aba"));
    ASSERT_FALSE(e.accept("bab"));
    ASSERT_FALSE(e.accept("abcabb"));
}


