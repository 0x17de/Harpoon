#ifndef TEST_H
#define TEST_H

#include <stdexcept>
#include <functional>
#include <list>


#define TEST(type, body)                                                \
    struct __REG##type {                                                \
        __REG##type() {                                                 \
            TestRunner::getInstance().addTest(__FILE__, #type, body);   \
        }                                                               \
    } __reg##type;

#define ASSERT_THROWS(type, body)                                 \
    TestRunner::assertThrows<type>(__FILE__, __LINE__, body)

#define ASSERT_NOTHROW(body)                              \
    TestRunner::assertNoThrow(__FILE__, __LINE__, body)

#define ASSERT_EQUAL(left, right)                               \
    TestRunner::assertEqual(__FILE__, __LINE__, left, right)

#define ASSERT(x)                                       \
    if (!x) throw new TestError(__FILE__, __LINE__);


class TestError : public std::runtime_error {
public:
    TestError(const std::string& file, size_t line);
    TestError(const std::string& file, size_t line, const std::string& reason);
};


class TestRunner {
    typedef std::tuple<std::string,
                       std::string,
                       std::function<void()>> TestData;
    std::list<TestData> tests;
public:
    TestRunner();
    static TestRunner& getInstance();

    void run();
    void addTest(const std::string& file, const std::string& name, std::function<void()>&& test) {
        tests.push_back(make_tuple(file,
                                   name,
                                   test));
    }

    template<class A>
    static void assertEqual(const std::string& file,
                            size_t line,
                            A a,
                            A b) {
        if (a != b)
            throw TestError(file, line);
    }

    static void assertNoThrow(const std::string& file,
                              size_t line,
                              std::function<void()> func);

    template<class Type>
    static void assertThrows(const std::string& file,
                             size_t line,
                             std::function<void()> func) {
        try {
            func();
        } catch(Type& type) {
            return;
        }
        throw TestError(file, line);
    }
};

#endif
