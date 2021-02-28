#include <gtest/gtest.h>
#include <Engine/Engine.hpp>
#include <string>
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
        Engine e("(a?)?");
        ASSERT_TRUE(e.accept(""));
        ASSERT_FALSE(e.accept("a?"));
        ASSERT_TRUE(e.accept("a"));
        ASSERT_FALSE(e.accept("aaa"));
        ASSERT_FALSE(e.accept("?"));
    }

    {
        Engine e("(ab)?([\\d]+)");
        ASSERT_TRUE(e.accept("1"));
        ASSERT_FALSE(e.accept("a"));
        ASSERT_TRUE(e.accept("ab6"));
        ASSERT_FALSE(e.accept("b"));
    }
    {
        Engine e("(xa?://)?[\\da-z]+");
        ASSERT_TRUE(e.accept("x://007"));
        ASSERT_TRUE(e.accept("007"));
        ASSERT_TRUE(e.accept("xa://w"));
        ASSERT_FALSE(e.accept("https//"));
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

TEST(EngineTest, accpetAnyDigit)
{
    {
    Engine e("(\\d)+");
    ASSERT_TRUE(e.accept("0"));
    ASSERT_TRUE(e.accept("9"));
    ASSERT_TRUE(e.accept("007"));
    
    ASSERT_FALSE(e.accept(""));
    ASSERT_FALSE(e.accept("\\d"));
    ASSERT_FALSE(e.accept("(\\d)+"));
    ASSERT_FALSE(e.accept("abcabb"));
    }
    {
    Engine e("00(\\d)?");
    ASSERT_TRUE(e.accept("00"));
    ASSERT_TRUE(e.accept("009"));
    ASSERT_TRUE(e.accept("007"));
    
    ASSERT_FALSE(e.accept(""));
    ASSERT_FALSE(e.accept("123"));
    ASSERT_FALSE(e.accept("00a"));
    ASSERT_FALSE(e.accept("00(\\d)?"));
    }
}


TEST(EngineTest, accpetAnyNotDigit)
{
    Engine e("(\\D)+");
    ASSERT_TRUE(e.accept("abc"));
    ASSERT_TRUE(e.accept("#$%)"));
    ASSERT_TRUE(e.accept("ABC "));
    ASSERT_TRUE(e.accept("(\\D)+"));
    
    ASSERT_FALSE(e.accept("123"));
    ASSERT_FALSE(e.accept("0"));
}

TEST(EngineTest, accpetAnyWord)
{
    Engine e("(\\w)+");
    ASSERT_TRUE(e.accept("abc"));
    ASSERT_TRUE(e.accept("123"));
    ASSERT_TRUE(e.accept("Ab_1"));	
    ASSERT_FALSE(e.accept("(\\D)+"));
    ASSERT_FALSE(e.accept("ABC "));
    ASSERT_FALSE(e.accept("#$%)"));
}

TEST(EngineTest, accpetAnyNotWord)
{
    Engine e("(\\W)+");
    ASSERT_TRUE(e.accept("*-+=)"));
    ASSERT_TRUE(e.accept("#$%)"));
    ASSERT_FALSE(e.accept("Ab_1"));	
    ASSERT_FALSE(e.accept("(\\D)+"));
    ASSERT_FALSE(e.accept("ABC "));
}

TEST(EngineTest, accpetCharGroup)
{
    Engine e("[\\da-z]+");
    ASSERT_TRUE(e.accept("az9"));
    ASSERT_TRUE(e.accept("9ad"));
    ASSERT_TRUE(e.accept("azs"));
    ASSERT_TRUE(e.accept("999"));
    ASSERT_FALSE(e.accept("Ab_1"));	
    ASSERT_FALSE(e.accept("(\\D)+"));
    ASSERT_FALSE(e.accept("a-z+"));
}

TEST(EngineTest, accpetRangeQuantifer)
{
    {
        Engine e("[\\da-z]{1,3}");
        ASSERT_TRUE(e.accept("az9"));
        ASSERT_FALSE(e.accept("az9d"));	
        ASSERT_FALSE(e.accept("a{1,3}"));
    }
    {
        Engine e("[\\da-z]{3,}");
        ASSERT_TRUE(e.accept("az9"));
        ASSERT_TRUE(e.accept("az999"));
        ASSERT_FALSE(e.accept("az"));	
        ASSERT_FALSE(e.accept("a{1,3}"));
    }
    {
        Engine e("[\\da-z]{0}");
        ASSERT_TRUE(e.accept(""));
        ASSERT_FALSE(e.accept("az9"));
        ASSERT_FALSE(e.accept("az999"));
    }
    {
        Engine e("[\\da-z]{2,6}");
        ASSERT_TRUE(e.accept("a99999"));
        ASSERT_TRUE(e.accept("9nn"));
        ASSERT_FALSE(e.accept("a0123456"));
    }
}

TEST(EngineTest, matchLineStartAndLineEnd)
{
    std::string vaild = "my-us3r_n4m3";
    std::string invaild = "th1s1s-wayt00_l0ngt0beausername";
    std::string input = vaild + '\n' + invaild + '\n';
    std::vector<std::string> testVector = {
        vaild
    };

    Engine e("^[a-z0-9_-]{3,16}$");
    std::vector<std::string> result = e.match(input);
    ASSERT_EQ(result, testVector);
}

TEST(EngineTest, matchLineStart)
{
    std::string vaild = "john@doe.com";
    std::string invaild = vaild + ".cn.org";
    std::string input = vaild + '\n' + invaild + '\n';
    std::vector<std::string> testVector = {
        vaild,
        vaild + ".cn"
    };

    Engine e("^([a-z0-9_-]+)@([\\da-z-]+)\\.([a-z.]{2,6})");
    std::vector<std::string> result = e.match(input);
    ASSERT_EQ(result, testVector);
}

TEST(EngineTest, matchLineEnd)
{
    std::string vaild = "starthttps://google.com/about";
    std::string invaild = "http://google.com/some/file!.html";
    std::string input = vaild + '\n' + invaild + '\n';
    std::vector<std::string> testVector = {
        "https://google.com/about"
    };

    Engine e("(https?://)?([\\da-z-]+)\\.([a-z.]{2,6})([/\\w .-]*)*/?$");
    ASSERT_TRUE(e.accept("https://google.com/about"));
    std::vector<std::string> result = e.match(input);
    ASSERT_EQ(result, testVector);
}

TEST(EngineTest, matchAnyWhere)
{
    std::string vaild = "starthttps://google.com/about";
    std::string invaild = "http://google.com/some/file!.htmlhttps://cpluscplus.com";
    std::string input = vaild + '\n' + invaild + '\n';
    std::vector<std::string> testVector = {
        "https://google.com/about",
        "http://google.com/some/file",
        "https://cpluscplus.com"
    };

    Engine e("(https?://)?([\\da-z-]+)\\.([a-z.]{2,6})([/\\w .-]*)*/?");
    std::vector<std::string> result = e.match(input);
    ASSERT_EQ(result, testVector);
}