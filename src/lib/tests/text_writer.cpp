
#include <gtest/gtest.h>
#include <lib/text_writer.h>

using namespace Lya::lib;

TEST(TextWriter_Init__InitText, ShouldInitializeWithAnEmptyText) {
    TextWriter w;
    EXPECT_EQ(*w.text, "");
}

TEST(TextWriter_Indent__SingleIndent, ShouldBeAbleToIndentText) {
    TextWriter w;
    w.indent();
    w.write("text");
    EXPECT_EQ(*w.text, "    text");
}

TEST(TextWriter_Indent__Indent, ShouldBeAbleToIndentMultipleTimes) {
    TextWriter w;
    w.indent();
    w.indent();
    w.write("text");
    EXPECT_EQ(*w.text, "        text");
}

TEST(TextWriter_Indent__Write_Indent, ShouldNotIndentWhenNoNewLineIsWritten) {
    TextWriter w;
    w.write("text1");
    w.indent();
    w.write("text2");
    EXPECT_EQ(*w.text, "text1text2");
}

TEST(TextWriter_Indent__WriteLine_Indent, ShouldIndentWhenNewlineIsWritten) {
    TextWriter w;
    w.write_line("text1");
    w.indent();
    w.write("text2");
    EXPECT_EQ(*w.text, "text1\n    text2");
}

TEST(TextWriter_Indent__WriteLine_Indent_WriteLine, ShouldIndentWhenNewlineIsWritten) {
    TextWriter w;
    w.write_line("text1");
    w.indent();
    w.write_line("text2");
    EXPECT_EQ(*w.text, "text1\n    text2\n");
}

TEST(TextWriter_Indent__Indent_Newline, ShouldNotPrintIndentationOnNewlineOnly) {
    TextWriter w;
    w.indent();
    w.newline();
    EXPECT_EQ(*w.text, "\n");
}

TEST(TextWriter_SaveRestore__Save, ShouldBeAbleToSave) {
    TextWriter w;
    w.save();
}

TEST(TextWriter_SaveRestore__Restore, ShouldThrowWhenOverRestoring) {
    TextWriter w;
    EXPECT_THROW(w.restore(), std::domain_error);
}

TEST(TextWriter_SaveRestore__Save_Restore, ShouldBeAbleToRestore) {
    TextWriter w;
    w.save();
    EXPECT_EQ(*w.text, "");
}

TEST(TextWriter_SaveRestore__Save_Write_Restore, ShouldBeAbleToRestoreLastSavedState) {
    TextWriter w;
    w.write("text1");
    w.save();
    w.write("text2");
    w.restore();
    EXPECT_EQ(*w.text, "text1");
}

TEST(TextWriter_SaveRestore__Save_Write_Save_Write_Restore, ShouldBeAbleToRestoreLastSavedState) {
    TextWriter w;
    w.write("text1");
    w.save();
    w.write("text2");
    w.save();
    w.write("text3");
    w.restore();
    EXPECT_EQ(*w.text, "text1text2");
}

TEST(TextWriter_SaveRestore__Save_Write_Save_Write_Restore_Restore, ShouldBeAbleToRestorePreviousLastSavedState) {
    TextWriter w;
    w.write("text1");
    w.save();
    w.write("text2");
    w.save();
    w.write("text3");
    w.restore();
    w.restore();
    EXPECT_EQ(*w.text, "text1");
}

TEST(TextWriter_SaveRestore__Save_Restore_Restore, ShouldThrowWhenOverRestoring) {
    TextWriter w;
    w.write("text");
    w.save();
    w.restore();
    try {
        w.restore();
        FAIL();
    }
    catch (std::domain_error& err) {
        EXPECT_STREQ(err.what(), "You have restored all saves.");
    }
}

TEST(TextWriter_Placeholder__BeginWriteOnPlaceholder, ShouldThrowWhenBeginWritingOnANonExistentPlaceholder) {
    TextWriter w;
    try {
        w.begin_write_on_placeholder("placeholder");
        FAIL();
    }
    catch (std::invalid_argument& err) {
        EXPECT_STREQ(err.what(), "Cannot find placeholder 'placeholder'.");
    }
}

TEST(TextWriter_Placeholder__EndWriteOnPlaceholder, CannotEndWriteOnPlaceholderWhenNoneHasBegun) {
    TextWriter w;
    try {
        w.end_write_on_placeholder();
        FAIL();
    }
    catch (std::domain_error& err) {
        EXPECT_STREQ(err.what(), "You have ended all started placeholders.");
    }
}

TEST(TextWriter_Placeholder__AddPlaceholder, ShouldBeAbleToAddPlaceholder) {
    TextWriter w;
    w.add_placeholder("placeholder");
    EXPECT_EQ(*w.text, "");
}

TEST(TextWriter_Placeholder__AddPlaceholder_AddPlaceholder, CanAddSubsequentPlaceholder) {
    TextWriter w;
    w.add_placeholder("placeholder1");
    w.add_placeholder("placeholder2");
    EXPECT_EQ(*w.text, "");
}

TEST(TextWriter_Placeholder__AddPlaceholder_BeginWriteOnPlaceholder, CanBeginWriteOnPlaceholder) {
    TextWriter w;
    w.add_placeholder("placeholder");
    w.begin_write_on_placeholder("placeholder");
    EXPECT_EQ(*w.text, "");
}

TEST(TextWriter_Placeholder__AddPlaceholder_BeginWriteOnPlaceholder_AddPlaceholder, CannotAddAPlaceholderWhenWritingToOne) {
    TextWriter w;
    w.add_placeholder("placeholder1");
    w.begin_write_on_placeholder("placeholder1");
    try {
        w.add_placeholder("placeholder2");
        FAIL();
    }
    catch (std::domain_error& err) {
        EXPECT_STREQ(err.what(), "Cannot add a placeholder when writing to one.");
    }
}

TEST(TextWriter_Placeholder__AddPlaceholder_BeginWriteOnPlaceholder_EndWriteOnPlaceholder_AddPlaceholder, CanAddPlaceholderAfterEndOfWriteOnPlaceholder) {
    TextWriter w;
    w.add_placeholder("placeholder");
    w.begin_write_on_placeholder("placeholder");
    w.end_write_on_placeholder();
    w.write("text");
    EXPECT_EQ(*w.text, "text");
}

TEST(TextWriter_Placeholder__AddPlaceholder_BeginWriteOnPlaceholder_EndWriteOnPlaceholder_Write, ShouldBeAbleToWriteAfterEndOfPlaceholder) {
    TextWriter w;
    w.add_placeholder("placeholder");
    w.begin_write_on_placeholder("placeholder");
    w.end_write_on_placeholder();
    w.write("text");
    EXPECT_EQ(*w.text, "text");
}

TEST(TextWriter_Placeholder__AddPlaceholder_BeginWriteOnPlaceholder_Write, CanWriteOnBegunPlaceholderWrite) {
    TextWriter w;
    w.add_placeholder("placeholder");
    w.begin_write_on_placeholder("placeholder");
    w.write("text");
    EXPECT_EQ(*w.text, "text");
}

TEST(TextWriter_Placeholder__AddPlaceholder_BeginWriteOnPlaceholder_BeginWriteOnPlaceholder, CannotBeginAnAlreadyBegunPlaceholderWrite) {
    TextWriter w;
    w.add_placeholder("placeholder");
    w.begin_write_on_placeholder("placeholder");
    try {
        w.begin_write_on_placeholder("placeholder");
        FAIL();
    }
    catch (std::invalid_argument& err) {
        EXPECT_STREQ(err.what(), "Cannot begin an already begun placeholder 'placeholder'.");
    }
}

TEST(TextWriter_Placeholder__AddPlaceholder_BeginWriteOnPlaceholder_Write_EndWriteOnPlaceholder, CanEndWriteOnPlaceholder) {
    TextWriter w;
    w.add_placeholder("placeholder");
    w.begin_write_on_placeholder("placeholder");
    w.write("text");
    w.end_write_on_placeholder();
    EXPECT_EQ(*w.text, "text");
}

TEST(TextWriter_Placeholder__AddPlaceholder_BeginWriteOnPlaceholder_Write_EndWriteOnPlaceholder_AddPlaceholder, CanWriteAfterAndOfWriteOnPlaceholder) {
    TextWriter w;
    w.add_placeholder("placeholder");
    w.begin_write_on_placeholder("placeholder");
    w.write("text1");
    w.end_write_on_placeholder();
    w.write("text2");
    EXPECT_EQ(*w.text, "text1text2");
}

TEST(TextWriter_Placeholder__AddPlaceholder_BeginWriteOnPlaceholder_EndWriteOnPlaceholder_EndWriteOnPlaceholder, CannotEndWriteOnPlaceholderMoreThanWhatHasStarted) {
    TextWriter w;
    w.add_placeholder("placeholder");
    w.begin_write_on_placeholder("placeholder");
    w.end_write_on_placeholder();
    try {
        w.end_write_on_placeholder();
        FAIL();
    }
    catch (std::domain_error& err) {
        EXPECT_STREQ(err.what(), "You have ended all started placeholders.");
    }
}

TEST(TextWriter_Placeholder__AddPlaceholder_EndWriteOnPlaceholder, CannotEndWriteOnPlaceholderWhenNoneHasBegun) {
    TextWriter w;
    w.add_placeholder("placeholder");
    try {
        w.end_write_on_placeholder();
        FAIL();
    }
    catch (std::domain_error& err) {
        EXPECT_STREQ(err.what(), "You have ended all started placeholders.");
    }
}

TEST(TextWriter_Placeholder__AddPlaceholder_Write_BeginWriteOnPlaceholder_Write, ShouldNotAppendTextIfEndOfWriteOnPlaceholderIsNotCalled) {
    TextWriter w;
    w.add_placeholder("placeholder");
    w.write("text1");
    w.begin_write_on_placeholder("placeholder");
    w.write("text2");
    EXPECT_EQ(*w.text, "text2");
}

TEST(TextWriter_Placeholder__AddPlaceholder_Write_BeginWriteOnPlaceholder_Write_EndWriteOnPlaceholder, ShouldApplyPlaceholderWriteOnPlaceholderAndAppendPriorTextAfterPlaceholderWrite) {
    TextWriter w;
    w.add_placeholder("placeholder");
    w.write("text1");
    w.begin_write_on_placeholder("placeholder");
    w.write("text2");
    w.end_write_on_placeholder();
    EXPECT_EQ(*w.text, "text2text1");
}

TEST(TextWriter_Placeholder__AddPlaceholder_Write_BeginWriteOnPlaceholder_Write_EndWriteOnPlaceholder_Write, CanWriteAfterEndOfPlaceholder) {
    TextWriter w;
    w.add_placeholder("placeholder");
    w.write("text1");
    w.begin_write_on_placeholder("placeholder");
    w.write("text2");
    w.end_write_on_placeholder();
    w.write("text3");
    EXPECT_EQ(*w.text, "text2text1text3");
}
