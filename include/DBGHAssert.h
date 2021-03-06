/**
 * @file        Assert.h
 * @author      Argishti Ayvazyan (ayvazyan.argishti@gmail.com)
 * @brief       Declaration for asserts.
 * @date        18-07-2020
 * @copyright   Copyright (c) 2020
 */

#pragma once

#include <format>

#include "impl/CAssertException.h"
#include "impl/CAssertConfig.h"
#include "impl/CAssertHandler.h"


#ifdef _MSC_VER

/**
 * @brief       START_DEBUGGING intrinsic at any point in your code. __debugbreak has the same effect as setting a breakpoint
 *               at that location using a debugger.
 *
 * @note        Working on Win32 platforms.
 */
#define START_DEBUGGING  __debugbreak()
#else

/**
 * @brief       START_DEBUGGING intrinsic at any point in your code. __asm__ volatile("int $0x03") has the same effect as
 *               setting a breakpoint at that location using a debugger.
 *
 * @note        Working on GNU platforms.
 */
#define START_DEBUGGING __asm__ volatile("int $0x03")
#endif


/**
 * @brief      The helper macro using for place code for asserts in one line.
 *
 * @param      _level_       The assert level.
 * @param      _expression_  Expression to be evaluated. If this expression evaluates to false, this causes an assertion failure.
 * @param      ...           The string and args for formating will appear as a runtime error if the _expression_ is false.
 */
#define IMPL_DBGH_ASSERT(_level_, _expression_, ...)                                                                                    \
    if ( dbgh::CAssertConfig::Get().IsActiveAssert(_level_) && ! bool(_expression_) )                                                   \
    {                                                                                                                                   \
        try {                                                                                                                           \
            dbgh::impl::CAssertHandler::HandleAssert<_level_>(std::format(__VA_ARGS__)                                                  \
                                                           , #_expression_ , __FILE__                                                   \
                                                           , __LINE__, __func__);                                                       \
        } catch (const dbgh::CAssertException& e) {                                                                                     \
            throw e;                                                                                                                    \
        }                                                                                                                               \
    }                                                                                                                                   \
    (void) 0


/**
 * @brief      The helper macro using for place code for asserts in one line.
 *              Specialization for ASSERT_DEBUG.
 *
 * @param      _level_       The assert level.
 * @param      _expression_  Expression to be evaluated. If this expression evaluates to false, this causes an assertion failure.
 * @param      ...           The string and args for formating will appear as a runtime error if the _expression_ is false.
 */
#define IMPL_DBGH_ASSERT_DEBUG(_level_, _expression_, ...)                                                                              \
    {                                                                                                                                   \
        static bool __ignore { false };                                                                                                 \
        if ( (! __ignore) && (dbgh::CAssertConfig::Get().IsActiveAssert(_level_)) && (! bool(_expression_)) )                           \
        {                                                                                                                               \
            try {                                                                                                                       \
                dbgh::impl::CAssertHandler::HandleAssert<_level_>(std::format(__VA_ARGS__)                                              \
                                                               , #_expression_ , __FILE__                                               \
                                                               , __LINE__, __func__, __ignore);                                         \
            } catch (const dbgh::impl::CAssertHandler::SStartDebuggingException) {                                                      \
                 START_DEBUGGING;                                                                                                       \
            } catch (const dbgh::CAssertException& e) {                                                                                 \
                throw e;                                                                                                                \
            }                                                                                                                           \
                                                                                                                                        \
        }                                                                                                                               \
    }                                                                                                                                   \
    (void) 0



/**
 * @brief      If the argument expression of this macro with functional form compares equal to 0 (i.e., the expression is false),
 *              this causes an assertion failure that by default prints the assertion information to std::cerr and prompt the user
 *              for action.
 *             There are available this actions:
 *              > (I/i) Ignore this assertion and continue execution.
 *              > (F/f) Ignore this assertion forever and continue execution.
 *              > (D/d) Break into the debugger if that attached otherwise behavior undefined.
 *              > (T/t) Throw \ref dbgh::CAssertException exception.
 *              > (B/b) Calls Terminate in \ref dbgh::CHandlerExecutor. By default, Terminate prints the assertion information
 *                  to std::cerr and call std::terminate.
 *
 * @example    The use example.
 *              ASSERT_DEBUG(vec.size() < 7, "The vector size always less than 7, the current size is: {}.", vec.size());
 *
 * @details    Prints information about assertion, the message contains:
 *              > Assertion type - DEBUG
 *              > Uncaught exc - The number of uncaught exceptions.
 *              > Filename - where the assertion is failed.
 *              > Function name - where the assertion is failed.
 *              > Expression - condition for the assertion that is failed.
 *              > Message - _message_ string passed to the assertion.
 *
 *
 * @details     The message formatting:
 *               The first argument std::string_view representing the format string. The format string consists of
 *                  > ordinary characters (except { and }), which are copied unchanged to the output,
 *                  > escape sequences {{ and }}, which are replaced with { and } respectively in the output, and
 *                  > replacement fields.
 *               Each replacement field has the following format:
 *                  > introductory { character;
 *                  > (optional) arg-id, a non-negative number;
 *                  > (optional) a colon (:) followed by a format specification;
 *                  > final } character.
 *              arg-id specifies the index of the argument in args whose value is to be used for formatting;
 *              if arg-id is omitted, the arguments are used in order. The arg-ids in a format string must all be
 *              present or all be omitted. Mixing manual and automatic indexing is an error.
 *
 *
 * @note       To change the assertion behavior, override the methods in the \ref dbgh::CHandlerExecutor class and set the new
 *              version to \ref dbgh::CAssertConfig, using the method \ref dbgh::CAssertConfig::SetExecutor for that.
 *              For more information, see the documentation for class \ref dbgh::CHandlerExecutor.
 *              The example below:
 *
 * @example     class NewExecutor : public dbgh::CHandlerExecutor { ... };
 *              dbgh::CAssertConfig::Get().SetExecutor(std::make_unique<NewExecutor>());
 *
 *
 * @param      _expression_  Expression to be evaluated. If this expression evaluates to false, this causes an assertion failure.
 * @param      ...           The string and args for formating will appear as a runtime error if the _expression_ is false.
 */
#define ASSERT_DEBUG(_expression_, ...)    IMPL_DBGH_ASSERT_DEBUG(dbgh::EAssertLevel::Debug, _expression_, __VA_ARGS__)

#ifndef DEBUG

/**
 * @brief      If the argument expression of this macro with functional form compares equal to 0 (i.e., the expression is false),
 *              this causes an assertion failure that calls HandleWarning in \ref dbgh::CHandlerExecutor.
 *              By default, HandleWarning prints the assertion information to std::cerr.
 *
 * @example    The use example.
 *              ASSERT_WARNING(vec.size() < 7, "The vector size always less than 7, the current size is: {}.", vec.size());
 *
 * @details    Prints information about assertion, the message contains:
 *              > Assertion type - WARNING
 *              > Uncaught exc - The number of uncaught exceptions.
 *              > Filename - where the assertion is failed.
 *              > Function name - where the assertion is failed.
 *              > Expression - condition for the assertion that is failed.
 *              > Message - _message_ string literal passed to the assertion.
 *
 *
 * @details     The message formatting:
 *               The first argument std::string_view representing the format string. The format string consists of
 *                  > ordinary characters (except { and }), which are copied unchanged to the output,
 *                  > escape sequences {{ and }}, which are replaced with { and } respectively in the output, and
 *                  > replacement fields.
 *               Each replacement field has the following format:
 *                  > introductory { character;
 *                  > (optional) arg-id, a non-negative number;
 *                  > (optional) a colon (:) followed by a format specification;
 *                  > final } character.
 *              arg-id specifies the index of the argument in args whose value is to be used for formatting;
 *              if arg-id is omitted, the arguments are used in order. The arg-ids in a format string must all be
 *              present or all be omitted. Mixing manual and automatic indexing is an error.
 *
 *
 * @note       To change the assertion behavior, override the methods in the \ref dbgh::CHandlerExecutor class and set the new
 *              version to \ref dbgh::CAssertConfig, using the method \ref dbgh::CAssertConfig::SetExecutor for that.
 *              For more information, see the documentation for class \ref dbgh::CHandlerExecutor.
 *              The example below:
 *
 * @example     class NewExecutor : public dbgh::CHandlerExecutor { ... };
 *              dbgh::CAssertConfig::Get().SetExecutor(std::make_unique<NewExecutor>());
 *
 * @param      _expression_  Expression to be evaluated. If this expression evaluates to false, this causes an assertion failure.
 * @param      ...           The string and args for formating will appear as a runtime error if the _expression_ is false.
 */
#define ASSERT_WARNING(_expression_, ...)  IMPL_DBGH_ASSERT(dbgh::EAssertLevel::Warning, _expression_, __VA_ARGS__)

/**
 * @brief      If the argument expression of this macro with functional form compares equal to 0 (i.e., the expression is false),
 *              this causes an assertion failure that calls HandleError in \ref dbgh::CHandlerExecutor.
 *              By default, HandleError prints the assertion information to std::cerr and throws \ref dbgh::CAssertException.
 *
 * @throw       throw  \ref dbgh::CAssertException  if causes an assertion failure.
 *
 * @example    The use example.
 *              ASSERT_ERROR(vec.size() < 7, "The vector size always less than 7, the current size is: {}.", vec.size());
 *
 * @details    Prints information about assertion, the message contains:
 *              > Assertion type - ERROR
 *              > Uncaught exc - The number of uncaught exceptions.
 *              > Filename - where the assertion is failed.
 *              > Function name - where the assertion is failed.
 *              > Expression - condition for the assertion that is failed.
 *              > Message - _message_ string literal passed to the assertion.
 *
 *
 * @details     The message formatting:
 *               The first argument std::string_view representing the format string. The format string consists of
 *                  > ordinary characters (except { and }), which are copied unchanged to the output,
 *                  > escape sequences {{ and }}, which are replaced with { and } respectively in the output, and
 *                  > replacement fields.
 *               Each replacement field has the following format:
 *                  > introductory { character;
 *                  > (optional) arg-id, a non-negative number;
 *                  > (optional) a colon (:) followed by a format specification;
 *                  > final } character.
 *              arg-id specifies the index of the argument in args whose value is to be used for formatting;
 *              if arg-id is omitted, the arguments are used in order. The arg-ids in a format string must all be
 *              present or all be omitted. Mixing manual and automatic indexing is an error.
 *
 *
 * @note       To change the assertion behavior, override the methods in the \ref dbgh::CHandlerExecutor class and set the new
 *              version to \ref dbgh::CAssertConfig, using the method \ref dbgh::CAssertConfig::SetExecutor for that.
 *              For more information, see the documentation for class \ref dbgh::CHandlerExecutor.
 *              The example below:
 *
 * @example     class NewExecutor : public dbgh::CHandlerExecutor { ... };
 *              dbgh::CAssertConfig::Get().SetExecutor(std::make_unique<NewExecutor>());
 *
 * @param      _expression_  Expression to be evaluated. If this expression evaluates to false, this causes an assertion failure.
 * @param      ...           The string and args for formating will appear as a runtime error if the _expression_ is false.
 */
#define ASSERT_ERROR(_expression_, ...)    IMPL_DBGH_ASSERT(dbgh::EAssertLevel::Error, _expression_, __VA_ARGS__)

/**
 * @brief      If the argument expression of this macro with functional form compares equal to 0 (i.e., the expression is false),
 *              this causes an assertion failure that calls Terminate in \ref dbgh::CHandlerExecutor.
 *              By default, Terminate prints the assertion information to std::cerr and call std::terminate.
 *
 * @note       By default ASSERT_FATAL disabled. To enable use \ref dbgh::CAssertConfig::EnableAsserts.
 * @example    dbgh::CAssertConfig::Get().EnableAsserts(dbgh::EAssertLevel::Fatal);
 *
 * @example    The use example.
 *              ASSERT_FATAL(vec.size() < 7, "The vector size always less than 7, the current size is: {}.", vec.size());
 *
 * @details    Prints information about assertion, the message contains:
 *              > Assertion type - FATAL
 *              > Uncaught exc - The number of uncaught exceptions.
 *              > Filename - where the assertion is failed.
 *              > Function name - where the assertion is failed.
 *              > Expression - condition for the assertion that is failed.
 *              > Message - _message_ string literal passed to the assertion.
 *
 *
 * @details     The message formatting:
 *               The first argument std::string_view representing the format string. The format string consists of
 *                  > ordinary characters (except { and }), which are copied unchanged to the output,
 *                  > escape sequences {{ and }}, which are replaced with { and } respectively in the output, and
 *                  > replacement fields.
 *               Each replacement field has the following format:
 *                  > introductory { character;
 *                  > (optional) arg-id, a non-negative number;
 *                  > (optional) a colon (:) followed by a format specification;
 *                  > final } character.
 *              arg-id specifies the index of the argument in args whose value is to be used for formatting;
 *              if arg-id is omitted, the arguments are used in order. The arg-ids in a format string must all be
 *              present or all be omitted. Mixing manual and automatic indexing is an error.
 *
 *
 * @note       To change the assertion behavior, override the methods in the \ref dbgh::CHandlerExecutor class and set the new
 *              version to \ref dbgh::CAssertConfig, using the method \ref dbgh::CAssertConfig::SetExecutor for that.
 *              For more information, see the documentation for class \ref dbgh::CHandlerExecutor.
 *              The example below:
 *
 * @example     class NewExecutor : public dbgh::CHandlerExecutor { ... };
 *              dbgh::CAssertConfig::Get().SetExecutor(std::make_unique<NewExecutor>());
 *
 * @param      _expression_  Expression to be evaluated. If this expression evaluates to false, this causes an assertion failure.
 * @param      .             The string and args for formating will appear as a runtime error if the _expression_ is false.
 */
#define ASSERT_FATAL(_expression_, ...)    IMPL_DBGH_ASSERT(dbgh::EAssertLevel::Fatal, _expression_, __VA_ARGS__)

#else

/**
 * @brief      The defines in debug mode.
 *
 * @details    In debug mode, all asserts replace to \ref ASSERT_DEBUG
 */
#define ASSERT_WARNING(_expression_, ...)  IMPL_DBGH_ASSERT_DEBUG(dbgh::EAssertLevel::Debug, _expression_, __VA_ARGS__)
#define ASSERT_ERROR(_expression_, ...)    IMPL_DBGH_ASSERT_DEBUG(dbgh::EAssertLevel::Debug, _expression_, __VA_ARGS__)
#define ASSERT_FATAL(_expression_, ...)    IMPL_DBGH_ASSERT_DEBUG(dbgh::EAssertLevel::Debug, _expression_, __VA_ARGS__)

#endif
