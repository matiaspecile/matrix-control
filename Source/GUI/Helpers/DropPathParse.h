#pragma once

#include <juce_core/juce_core.h>

namespace TSS
{
    namespace DropPathParseDetail
    {
        inline juce::String stripOptionalUriListBrackets(juce::String trimmed)
        {
            // RFC 2483 uri-list may wrap URIs in angle brackets.
            if (trimmed.startsWithChar('<') && trimmed.endsWithChar('>') && trimmed.length() >= 2)
                return trimmed.substring(1, trimmed.length() - 1).trim();

            return trimmed;
        }

        inline bool tryAssignLocalFilePath(const juce::File& file, juce::String& outPath)
        {
            const auto fullPath = file.getFullPathName();

            if (fullPath.isEmpty())
                return false;

            outPath = fullPath;
            return true;
        }

        inline bool tryParseFileSchemeLine(const juce::String& trimmed, juce::String& outPath)
        {
            const juce::URL url(trimmed);

            if (! url.isLocalFile())
                return false;

            return tryAssignLocalFilePath(url.getLocalFile(), outPath);
        }

        inline bool tryParseAbsolutePathLine(const juce::String& trimmed, juce::String& outPath)
        {
            if (! juce::File::isAbsolutePath(trimmed))
                return false;

            return tryAssignLocalFilePath(juce::File(trimmed), outPath);
        }
    }

    /** Parse one uri-list / absolute-path line into a local filesystem path.
        Returns false for comments, blank lines, non-file URLs, and relative paths. */
    inline bool tryParseDropTextLineToPath(const juce::String& line, juce::String& outPath)
    {
        const auto trimmed = DropPathParseDetail::stripOptionalUriListBrackets(line.trim());

        if (trimmed.isEmpty() || trimmed.startsWithChar('#'))
            return false;

        if (trimmed.contains("://") && ! trimmed.startsWithIgnoreCase("file:"))
            return false;

        if (trimmed.startsWithIgnoreCase("file:"))
            return DropPathParseDetail::tryParseFileSchemeLine(trimmed, outPath);

        return DropPathParseDetail::tryParseAbsolutePathLine(trimmed, outPath);
    }

    /** Parse OS text DnD payloads (uri-list / absolute paths) into local file paths.
        Returns an empty array when the text contains no usable paths (caller ignores). */
    inline juce::StringArray parseDropTextToFilePaths(const juce::String& text)
    {
        juce::StringArray paths;
        juce::StringArray lines;
        lines.addLines(text);

        for (const auto& line : lines)
        {
            juce::String path;

            if (tryParseDropTextLineToPath(line, path))
                paths.addIfNotAlreadyThere(path);
        }

        return paths;
    }
}
