//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03

// <experimental/filesystem>

// void permissions(const path& p, perms prms);
// void permissions(const path& p, perms prms, std::error_code& ec) noexcept;


#include <experimental/filesystem>

#include "test_macros.h"
#include "rapid-cxx-test.hpp"
#include "filesystem_test_helper.hpp"

using namespace std::experimental::filesystem;
namespace fs = std::experimental::filesystem;

using PR = fs::perms;

TEST_SUITE(filesystem_permissions_test_suite)

TEST_CASE(test_signatures)
{
    const path p; ((void)p);
    const perms opts{}; ((void)opts);
    std::error_code ec; ((void)ec);
    ASSERT_NOT_NOEXCEPT(fs::permissions(p, opts));
    // Not noexcept because of narrow contract
    LIBCPP_ONLY(
        ASSERT_NOT_NOEXCEPT(fs::permissions(p, opts, ec)));
}

TEST_CASE(test_error_reporting)
{
    auto checkThrow = [](path const& f, fs::perms opts, const std::error_code& ec)
    {
#ifndef TEST_HAS_NO_EXCEPTIONS
        try {
            fs::permissions(f, opts);
            return false;
        } catch (filesystem_error const& err) {
            return err.path1() == f
                && err.path2() == ""
                && err.code() == ec;
        }
#else
        ((void)f); ((void)opts); ((void)ec);
        return true;
#endif
    };

    scoped_test_env env;
    const path dne = env.make_env_path("dne");
    const path dne_sym = env.create_symlink(dne, "dne_sym");
    { // !exists
        std::error_code ec;
        fs::permissions(dne, fs::perms{}, ec);
        TEST_REQUIRE(ec);
        TEST_CHECK(checkThrow(dne, fs::perms{}, ec));
    }
    {
        std::error_code ec;
        fs::permissions(dne_sym, fs::perms{}, ec);
        TEST_REQUIRE(ec);
        TEST_CHECK(checkThrow(dne_sym, fs::perms{}, ec));
    }
}

TEST_CASE(basic_permissions_test)
{
    scoped_test_env env;
    const path file = env.create_file("file1", 42);
    const path dir = env.create_dir("dir1");
    const path file_for_sym = env.create_file("file2", 42);
    const path sym = env.create_symlink(file_for_sym, "sym");
    const perms AP = perms::add_perms;
    const perms RP = perms::remove_perms;
    const perms NF = perms::symlink_nofollow;
    struct TestCase {
      path p;
      perms set_perms;
      perms expected;
    } cases[] = {
        // test file
        {file, perms::none, perms::none},
        {file, perms::owner_all, perms::owner_all},
        {file, perms::group_all | AP, perms::owner_all | perms::group_all},
        {file, perms::group_all | RP, perms::owner_all},
        // test directory
        {dir, perms::none, perms::none},
        {dir, perms::owner_all, perms::owner_all},
        {dir, perms::group_all | AP, perms::owner_all | perms::group_all},
        {dir, perms::group_all | RP, perms::owner_all},
        // test symlink without symlink_nofollow
        {sym, perms::none, perms::none},
        {sym, perms::owner_all, perms::owner_all},
        {sym, perms::group_all | AP, perms::owner_all | perms::group_all},
        {sym, perms::group_all | RP , perms::owner_all},
        // test non-symlink with symlink_nofollow. The last test on file/dir
        // will have set their permissions to perms::owner_all
        {file, perms::group_all | AP | NF, perms::owner_all | perms::group_all},
        {dir,  perms::group_all | AP | NF, perms::owner_all | perms::group_all}
    };
    for (auto const& TC : cases) {
        TEST_CHECK(status(TC.p).permissions() != TC.expected);
        // Set the error code to ensure it's cleared.
        std::error_code ec = std::make_error_code(std::errc::bad_address);
        permissions(TC.p, TC.set_perms, ec);
        TEST_CHECK(!ec);
        auto pp = status(TC.p).permissions();
        TEST_CHECK(pp == TC.expected);
    }
}

TEST_CASE(test_no_resolve_symlink_on_symlink)
{
    scoped_test_env env;
    const path file = env.create_file("file", 42);
    const path sym = env.create_symlink(file, "sym");
    const auto file_perms = status(file).permissions();

    struct TestCase {
        perms set_perms;
        perms expected; // only expected on platform that support symlink perms.
    } cases[] = {
        {perms::owner_all, perms::owner_all},
        {perms::group_all | perms::add_perms, perms::owner_all | perms::group_all},
        {perms::owner_all | perms::remove_perms, perms::group_all},
    };
    for (auto const& TC : cases) {
#if defined(__APPLE__) || defined(__FreeBSD__)
        // On OS X symlink permissions are supported. We should get an empty
        // error code and the expected permissions.
        const auto expected_link_perms = TC.expected;
        std::error_code expected_ec;
#else
        // On linux symlink permissions are not supported. The error code should
        // be 'operation_not_supported' and the sylink permissions should be
        // unchanged.
        const auto expected_link_perms = symlink_status(sym).permissions();
        std::error_code expected_ec = std::make_error_code(std::errc::operation_not_supported);
#endif
        std::error_code ec = std::make_error_code(std::errc::bad_address);
        permissions(sym, TC.set_perms | perms::symlink_nofollow, ec);
        TEST_CHECK(ec == expected_ec);
        TEST_CHECK(status(file).permissions() == file_perms);
        TEST_CHECK(symlink_status(sym).permissions() == expected_link_perms);
    }
}

TEST_SUITE_END()
