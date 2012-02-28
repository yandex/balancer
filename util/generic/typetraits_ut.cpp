#include "typetraits.h"

#include <library/unittest/registar.h>

class TTypeTraitsTest: public TTestBase {
        UNIT_TEST_SUITE(TTypeTraitsTest);
            UNIT_TEST(TestUnsignedChar)
            UNIT_TEST(TestDouble)
            UNIT_TEST(TestLongDouble)
            UNIT_TEST(TestSizeT)
            UNIT_TEST(TestClass)
            UNIT_TEST(TestReference)
        UNIT_TEST_SUITE_END();
    private:
        inline void TestReference() {
            UNIT_ASSERT(TTypeTraits<Stroka&>::IsReference);
            //UNIT_ASSERT(TTypeTraits<const Stroka&>::IsConstReference);
        }

        inline void TestClass() {
            UNIT_ASSERT(TTypeTraits<Stroka>::IsClassType);
        }

        inline void TestUnsignedChar() {
            TestArithmeticType<unsigned char>();
        }

        inline void TestSizeT() {
            TestArithmeticType<size_t>();
        }

        inline void TestDouble() {
            TestArithmeticType<double>();
        }

        inline void TestLongDouble() {
            TestArithmeticType<long double>();
        }

        template <class T>
        inline void TestArithmeticType() {
            UNIT_ASSERT(TTypeTraits<T>::IsArithmetic);
            UNIT_ASSERT(TTypeTraits<const T>::IsArithmetic);
            UNIT_ASSERT(TTypeTraits<volatile T>::IsArithmetic);
            UNIT_ASSERT(TTypeTraits<const volatile T>::IsArithmetic);

            UNIT_ASSERT(TTypeTraits<T>::IsValueType);
            UNIT_ASSERT(TTypeTraits<const T>::IsValueType);
            UNIT_ASSERT(TTypeTraits<volatile T>::IsValueType);
            UNIT_ASSERT(TTypeTraits<const volatile T>::IsValueType);

            bool a;

            a = TSameType<typename TTypeTraits<T>::TFuncParam, T>::Result;
            UNIT_ASSERT(a);
            a = TSameType<typename TTypeTraits<const volatile T>::TFuncParam, const volatile T>::Result;
            UNIT_ASSERT(a);
        }
};

UNIT_TEST_SUITE_REGISTRATION(TTypeTraitsTest);
