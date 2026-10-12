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
    static juce::File tempChild(const juce::String& fileName)
    {
        return juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile(fileName);
    }

    static juce::String fileUri(const juce::File& file)
    {
        return juce::URL(file).toString(false);
    }

    void parsesFileUri()
    {
        beginTest("parsesFileUri - file:/// absolute path");

        const auto file = tempChild("mc-drop-parse-patch.syx");
        const auto paths = TSS::parseDropTextToFilePaths(fileUri(file));
        expectEquals(paths.size(), 1);
        expectEquals(paths[0], file.getFullPathName());
    }

    void parsesBracketedFileUri()
    {
        beginTest("parsesBracketedFileUri - RFC uri-list angle brackets");

        const auto file = tempChild("mc-drop-parse-bracket.syx");
        const auto paths = TSS::parseDropTextToFilePaths("<" + fileUri(file) + ">");
        expectEquals(paths.size(), 1);
        expectEquals(paths[0], file.getFullPathName());
    }

    void parsesMultiLineUriList()
    {
        beginTest("parsesMultiLineUriList - CRLF uri-list with comment");

        const auto fileA = tempChild("mc-drop-parse-a.syx");
        const auto fileB = tempChild("mc-drop-parse-b.m1km");
        const juce::String text =
            "# comment\r\n"
            + fileUri(fileA) + "\r\n"
            + fileUri(fileB) + "\r\n";

        const auto paths = TSS::parseDropTextToFilePaths(text);
        expectEquals(paths.size(), 2);
        expectEquals(paths[0], fileA.getFullPathName());
        expectEquals(paths[1], fileB.getFullPathName());
    }

    void parsesAbsolutePath()
    {
        beginTest("parsesAbsolutePath - bare absolute filesystem path");

        const auto file = tempChild("mc-drop-parse-master.syx");
        const auto paths = TSS::parseDropTextToFilePaths(file.getFullPathName() + "\n");
        expectEquals(paths.size(), 1);
        expectEquals(paths[0], file.getFullPathName());
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

        const auto file = tempChild("mc-drop-parse My Patch.syx");
        const auto paths = TSS::parseDropTextToFilePaths(fileUri(file));
        expectEquals(paths.size(), 1);
        expectEquals(paths[0], file.getFullPathName());
    }
};

static DropPathParseTests dropPathParseTests;
