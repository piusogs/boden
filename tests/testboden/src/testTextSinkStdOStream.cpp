#include <bdn/init.h>
#include <bdn/test.h>

#include <bdn/TextSinkStdOStream.h>

#include <bdn/test/MockTextUi.h>

using namespace bdn;

class TestTextSinkForTextSinkStdOStream : public Base, BDN_IMPLEMENTS ITextSink
{
  public:
    Array<String> writtenChunks;

    void write(const String &s) override { writtenChunks.add(s); }

    void writeLine(const String &s) override { writtenChunks.add(s + "\n"); }
};

template <class CharType> static void testTextSinkStdOStream()
{
    P<TestTextSinkForTextSinkStdOStream> sink = newObj<TestTextSinkForTextSinkStdOStream>();

    const Array<String> &writtenChunks = sink->writtenChunks;

    TextSinkStdOStream<CharType> streamObj(sink);

    // use the classic locale so that we get predictable formatting
    streamObj.imbue(std::locale::classic());

    // most of the detailed tests for decoding corner cases etc. are covered by
    // the tests for TextUiStdStreamBuf. Here we only have some rough tests that
    // verify that the integration of the streambuf into a stream object works
    // as expected.

    // to be certain that this is the case we use the stream as a generic
    // std::ostream.
    std::basic_ostream<CharType> &stream = streamObj;

    SECTION("several values without linebreak")
    {
        stream << 42 << " " << 1.22 << " " << String("hello world");

        // should be buffered initially.
        REQUIRE(writtenChunks.size() == 0);

        stream.flush();

        REQUIRE(writtenChunks.size() == 1);
        REQUIRE(writtenChunks[0] == "42 1.22 hello world");
    }

    SECTION("several values with linebreak")
    {
        stream << 42 << " " << 1.22 << std::endl << String("hello world");

        // the implementation flushes at line breaks.
        REQUIRE(writtenChunks.size() == 1);
        REQUIRE(writtenChunks[0] == "42 1.22\n");

        stream.flush();

        REQUIRE(writtenChunks.size() == 2);
        REQUIRE(writtenChunks[1] == "hello world");
    }

    SECTION("autosync when buffer exceeded")
    {
        // the internal buffer size is 64 chars
        for (int i = 0; i < 8; i++)
            stream << "01234567";

        // should still fit into the buffer
        REQUIRE(writtenChunks.size() == 0);

        // this exceeds the buffer and should cause a flush
        stream << "X";

        REQUIRE(writtenChunks.size() == 1);
        REQUIRE(writtenChunks[0] == "0123456701234567012345670123456701234567012345670123456701234567");
    }
}

TEST_CASE("TextUiStdOStream")
{
    SECTION("char")
    testTextSinkStdOStream<char>();

    SECTION("wchar_t")
    testTextSinkStdOStream<wchar_t>();

    // the std stream implementations for char16_t and char32_t do not work
    // reliably with all compilers at the time of this writing (buggy in VS2015
    // and VS2017). Instead of trying to work around that we consider these
    // variants as "off limits" for the time being and do not test them.

    /*

    SECTION("char16_t")
        testTextUiStdOStream<char16_t>();

    SECTION("char32_t")
        testTextUiStdOStream<char32_t>();

        */
}
