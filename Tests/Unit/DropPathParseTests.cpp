#include <juce_core/juce_core.h>

#include "GUI/Helpers/DropPathParse.h"

class DropPathParseTests : public juce::UnitTest
{
public:
    DropPathParseTests()
        : juce::UnitTest("DropPathParse")
    {
    }

    void runTest() override
    {
        parsesFileUri();
        parsesBracketedFileUri();
        parsesMultiLineUriList();
        parsesAbsolutePath();
        rejectsNonPathText();
        rejectsNonFileUrlsAndRelativePaths();
        decodesPercentEncodedFileUri();
    }

private:
    void parsesFileUri()
    {
        beginTest("parsesFileUri - file:/// absolute path");

        const auto paths = TSS::parseDropTextToFilePaths("file:///tmp/patch.syx");
        expectEquals(paths.size(), 1);
        expectEquals(paths[0], juce::File("/tmp/patch.syx").getFullPathName());
    }

    void parsesBracketedFileUri()
    {
        beginTest("parsesBracketedFileUri - RFC uri-list angle brackets");

        const auto paths = TSS::parseDropTextToFilePaths("<file:///tmp/patch.syx>");
        expectEquals(paths.size(), 1);
        expectEquals(paths[0], juce::File("/tmp/patch.syx").getFullPathName());
    }

    void parsesMultiLineUriList()
    {
        beginTest("parsesMultiLineUriList - CRLF uri-list with comment");

        const juce::String text =
            "# comment\r\n"
            "file:///tmp/a.syx\r\n"
            "file:///tmp/b.m1km\r\n";

        const auto paths = TSS::parseDropTextToFilePaths(text);
        expectEquals(paths.size(), 2);
        expectEquals(paths[0], juce::File("/tmp/a.syx").getFullPathName());
        expectEquals(paths[1], juce::File("/tmp/b.m1km").getFullPathName());
    }

    void parsesAbsolutePath()
    {
        beginTest("parsesAbsolutePath - bare absolute filesystem path");

        const auto paths = TSS::parseDropTextToFilePaths("/home/user/master.syx\n");
        expectEquals(paths.size(), 1);
        expectEquals(paths[0], juce::File("/home/user/master.syx").getFullPathName());
    }

    void rejectsNonPathText()
    {
        beginTest("rejectsNonPathText - prose and empty ignored");

        expect(TSS::parseDropTextToFilePaths({}).isEmpty());
        expect(TSS::parseDropTextToFilePaths("hello world").isEmpty());
        expect(TSS::parseDropTextToFilePaths("DROP TO LOAD").isEmpty());
        expect(TSS::parseDropTextToFilePaths("# only a comment\n").isEmpty());
    }

    void rejectsNonFileUrlsAndRelativePaths()
    {
        beginTest("rejectsNonFileUrlsAndRelativePaths");

        expect(TSS::parseDropTextToFilePaths("https://example.com/a.syx").isEmpty());
        expect(TSS::parseDropTextToFilePaths("http://localhost/a.syx").isEmpty());
        expect(TSS::parseDropTextToFilePaths("relative/patch.syx").isEmpty());
        expect(TSS::parseDropTextToFilePaths("patch.syx").isEmpty());
    }

    void decodesPercentEncodedFileUri()
    {
        beginTest("decodesPercentEncodedFileUri - spaces in path");

        const auto paths = TSS::parseDropTextToFilePaths("file:///tmp/My%20Patch.syx");
        expectEquals(paths.size(), 1);
        expectEquals(paths[0], juce::File("/tmp/My Patch.syx").getFullPathName());
    }
};

static DropPathParseTests dropPathParseTests;
