/**
 * @file InternetMessageTests.cpp
 * 
 * This module contains unit Tests of the InternetMessage::InternetMessage class 
 * 
 * © 2024 by Hatem Nabli
*/

#include <gtest/gtest.h>
#include <InternetMessage/InternetMessage.hpp>
#include <string>
#include <vector>

TEST(InternetMessageTests, PlaceHolder_Test) {
    InternetMessage::InternetMessage imf;
    ASSERT_TRUE(true);
}

TEST(InternetMessageTests, HttpClientRequestMessage) {
    InternetMessage::InternetMessage imf;
    const std::string rawMessage = "User-Agent: curl/7.16.3 libcurl/7.163 OpenSSL/0.9.7l zlib/1.2.3\r\n"
        "Host: www.example.com\r\n"
        "Accept-Language: en, mi\r\n"
        "\r\n";
    ASSERT_TRUE(imf.ParseFromString(rawMessage));
    const auto headers = imf.GetHeaders();
    struct ExpectedHeader {
        std::string name;
        std::string value;
    };
    const std::vector< ExpectedHeader > expectedHeaders {
        {"User-Agent", "curl/7.16.3 libcurl/7.163 OpenSSL/0.9.7l zlib/1.2.3"},
        {"Host", "www.example.com"},
        {"Accept-Language", "en, mi"},
    };
    
    ASSERT_EQ(expectedHeaders.size(), headers.size());
    for(size_t i = 0; i < expectedHeaders.size(); ++i) {
        ASSERT_EQ(expectedHeaders[i].name, headers[i].name);
        ASSERT_EQ(expectedHeaders[i].value, headers[i].value);
    }
    ASSERT_TRUE(imf.HasHeader("Host"));
    ASSERT_FALSE(imf.HasHeader("Toto"));
    ASSERT_TRUE(imf.GetBody().empty());
}

TEST(InternetMessageTests, HttpServerResponseMessage) {
    InternetMessage::InternetMessage imf;
    const std::string rawMessage = "Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
            "Server: Apache\r\n"
            "Last-Modified: Wed, 22 Jul 2009 18:12:53 GMT\r\n"
            "ETag: \"34aa387-d-1568eb00\"\r\n"
            "Accept-Ranges: bytes\r\n"
            "Content-Length: 51\r\n"
            "Vary: Accept-Encoding\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "Hello World! My payload includes a trailling CRLF.\r\n";
    ASSERT_TRUE(
        imf.ParseFromString(rawMessage)
    );
    const auto headers = imf.GetHeaders();
    struct ExpectedHeader {
        std::string name;
        std::string value;
    };
    const std::vector< ExpectedHeader > expectedHeaders {
        {"Date", "Mon, 27 Jul 2009 12:28:53 GMT"},
        {"Server", "Apache"},
        {"Last-Modified", "Wed, 22 Jul 2009 18:12:53 GMT"},
        {"ETag", "\"34aa387-d-1568eb00\""},
        {"Accept-Ranges", "bytes"},
        {"Content-Length", "51"},
        {"Vary", "Accept-Encoding"},
        {"Content-Type", "text/plain"}
    };
    ASSERT_EQ(expectedHeaders.size(), headers.size());
    for(size_t i = 0; i < expectedHeaders.size(); ++i) {
        ASSERT_EQ(expectedHeaders[i].name, headers[i].name);
        ASSERT_EQ(expectedHeaders[i].value, headers[i].value);
    }
    ASSERT_TRUE(imf.HasHeader("Date"));
    ASSERT_FALSE(imf.HasHeader("Toto"));
    ASSERT_EQ("Hello World! My payload includes a trailling CRLF.\r\n", imf.GetBody());
    ASSERT_EQ(rawMessage, imf.GenerateRawMessage());
}

TEST(InternetMessageTests, HeadersLineAlmostTooLong) {
    InternetMessage::InternetMessage imf;
    const std::string testHeaderName("X-Poggers");
    const std::string testHeaderNameWithDelimiters = testHeaderName + ": ";
    const std::string longestPossiblePoggers(998 - testHeaderNameWithDelimiters.length(), 'X');
    const std::string rawMessage = (
        "User-Agent: curl/7.16.3 libcurl/7.163 OpenSSL/0.9.7l zlib/1.2.3\r\n"
        "Host: www.example.com\r\n"
        + testHeaderNameWithDelimiters + longestPossiblePoggers + "\r\n"
        "Accept-Language: en, mi\r\n"
        "\r\n"
    );
    ASSERT_TRUE(imf.ParseFromString(rawMessage));
    ASSERT_EQ(longestPossiblePoggers, imf.GetHeaderValue(testHeaderName));
}

TEST(InternetMessageTests, HeadersLineTooLong) {
    InternetMessage::InternetMessage imf;
    const std::string testHeaderName("X-Poggers");
    const std::string testHeaderNameWithDelimiters = testHeaderName + ": ";
    const std::string longestPossiblePoggers(999 - testHeaderNameWithDelimiters.length(), 'X');
    const std::string rawMessage = (
        "User-Agent: curl/7.16.3 libcurl/7.163 OpenSSL/0.9.7l zlib/1.2.3\r\n"
        "Host: www.example.com\r\n"
        + testHeaderNameWithDelimiters + longestPossiblePoggers + "\r\n"
        "Accept-Language: en, mi\r\n"
        "\r\n"
    );
    ASSERT_FALSE(imf.ParseFromString(rawMessage));
}

TEST(InternetMeassageTests, GetValueOfMissingHeader) {
    InternetMessage::InternetMessage imf;
    const std::string rawMessage = "User-Agent: curl/7.16.3 libcurl/7.163 OpenSSL/0.9.7l zlib/1.2.3\r\n"
        "Host: www.example.com\r\n"
        "Accept-Language: en, mi\r\n"
        "\r\n";
    ASSERT_TRUE(imf.ParseFromString(rawMessage));
    ASSERT_EQ("", imf.GetHeaderValue("toto"));
}

TEST(InternetLMessageTests, HeaderWithNonAsciiCharacterIntheName) {
    InternetMessage::InternetMessage imf;
    const std::string rawMessage = "User-Agent: curl/7.16.3 libcurl/7.163 OpenSSL/0.9.7l zlib/1.2.3\r\n"
        "Host: www.example.com\r\n"
        "Feels Bad: It's really Bad"
        "Accept-Language: en, mi\r\n"
        "\r\n";
    ASSERT_FALSE(imf.ParseFromString(rawMessage));
}

TEST(InternetLMessageTests, BodyWithLoneCRInMiddle) {
    InternetMessage::InternetMessage imf;
    const std::string rawMessage = "User-Agent: curl/7.16.3 libcurl/7.163 OpenSSL/0.9.7l zlib/1.2.3\r\n"
        "Host: www.example.com\r\n"
        "Accept-Language: en, mi\r\n"
        "\r\n"
        "Hello World! My payload\rincludes a trailling";
    ASSERT_FALSE(imf.ParseFromString(rawMessage));
}

TEST(InternetLMessageTests, BodyWithLoneCRAtTheEnd) {
    InternetMessage::InternetMessage imf;
    const std::string rawMessage = "User-Agent: curl/7.16.3 libcurl/7.163 OpenSSL/0.9.7l zlib/1.2.3\r\n"
        "Host: www.example.com\r\n"
        "Accept-Language: en, mi\r\n"
        "\r\n"
        "Hello World! My payload includes a trailling\r";
    ASSERT_FALSE(imf.ParseFromString(rawMessage));
}

TEST(InternetLMessageTests, BodyWithLoneLF) {
    InternetMessage::InternetMessage imf;
    const std::string rawMessage = "User-Agent: curl/7.16.3 libcurl/7.163 OpenSSL/0.9.7l zlib/1.2.3\r\n"
        "Host: www.example.com\r\n"
        "Accept-Language: en, mi\r\n"
        "\r\n"
        "Hello World! My payload\nincludes a trailling";
    ASSERT_FALSE(imf.ParseFromString(rawMessage));
}