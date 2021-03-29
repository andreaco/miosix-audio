#include "catch.hpp"
#include "../containers/circular_buffer.h"

TEST_CASE("CircularBuffer", "[containers]") {

    SECTION("CircularBuffer STL style") {
        CircularBuffer<int, 4> buffer;

        SECTION("putting elements in the buffer") {
            buffer.push_back(1);
            REQUIRE(buffer.front() == 1);

            buffer.push_back(2);
            REQUIRE(buffer.front() == 1);


            SECTION("getting elements") {
                REQUIRE(buffer.front() == 1);
                buffer.pop_front();
                REQUIRE(buffer.front() == 2);
                buffer.pop_front();
                REQUIRE(buffer.empty() == true);
            }


            SECTION("checking isEmpty") {
                REQUIRE(buffer.empty() == false);

                // clearing the buffer
                buffer.clear();
                REQUIRE(buffer.empty() == true);
            }

            SECTION("checking the buffer length") {
                REQUIRE(buffer.max_size() == 4);
            }

            SECTION("checking the current capacity") {
                REQUIRE(buffer.size() == 2);
                buffer.push_back(45);
                REQUIRE(buffer.size() == 3);
            }
        }
    }

    SECTION("Overriding CL") {
        CircularBuffer<float, 4> buffer;
        SECTION("wrapping") {
            REQUIRE(buffer.empty() == true);

            buffer.push_back(1);
            buffer.push_back(2);
            buffer.push_back(2.5);


            buffer.pop_front();
            buffer.pop_front();
            buffer.push_back(3.5);

            REQUIRE(buffer.size() == 2);

            buffer.push_back(3.6);

            buffer.push_back(3.9);

            buffer.pop_front();
            buffer.pop_front();

            REQUIRE(buffer.front() == Approx(3.6));
            buffer.pop_front();
            buffer.clear();
            REQUIRE(buffer.empty() == true);
            REQUIRE(buffer.size() == 0);
        }

        SECTION("overriding") {
            buffer.push_back(1);
            buffer.push_back(2);
            buffer.push_back(2.5);
            buffer.push_back(3.5);
            buffer.push_back(4.5);
            REQUIRE(buffer.front() == Approx(2));
        }

    }

    SECTION("Discarding CircularBuffer test") {
        CircularBuffer<double, 4, CircularBufferType::Discard> buffer;
        SECTION("wrapping") {
            buffer.push_back(1);
            buffer.push_back(2);
            buffer.push_back(2.5);
            buffer.pop_front();
            buffer.pop_front();
            buffer.push_back(3.5);
            REQUIRE(buffer.size() == 2);
            buffer.push_back(3.6);
            buffer.push_back(3.9);
            buffer.pop_front();
            buffer.pop_front();
            REQUIRE(buffer.front() == 3.6);
            buffer.pop_front();
            REQUIRE(buffer.size() == 1);
            buffer.clear();
            REQUIRE(buffer.empty() == true);
            REQUIRE(buffer.size() == 0);
        }

        SECTION("overriding") {
            buffer.push_back(1);
            buffer.push_back(2);
            buffer.push_back(2.5);
            buffer.push_back(3.5);
            buffer.push_back(4.5);
            REQUIRE(buffer.front() == 1);
            buffer.pop_front();
            REQUIRE(buffer.size() == 3);
        }
    }

    SECTION("Iterator") {
        SECTION("Explicit iterator use") {
            CircularBuffer<double, 6, CircularBufferType::Overwrite> buffer;
            double testValues[6] = {0, 1, 2, 3, 4, 5};

            for (auto i = 0; i < 6; ++i) {
                buffer.push_back(testValues[i]);
            }
            auto it = buffer.begin();
            auto end = buffer.end();
            auto i = 0;
            for (; it != end; ++it, ++i) {
                REQUIRE(*it == testValues[i]);
            }
        }

        SECTION("For each") {
            CircularBuffer<double, 6, CircularBufferType::Overwrite> buffer;
            double testValues[6] = {0, 1, 2, 3, 4, 5};

            for (auto i = 0; i < 6; ++i) {
                buffer.push_back(testValues[i]);
            }

            auto i = 0;
            for (auto item : buffer) {
                REQUIRE(item == testValues[i]);
                i++;
            }
        }

        SECTION ("std copy") {
            int ints[] = {10, 20, 30, 40, 50, 60};
            std::vector<int> vector1 (6);
            std::vector<int> vector2 (6);


            CircularBuffer<double, 6, CircularBufferType::Overwrite> buffer;
            for(auto i=0; i<6; ++i) {
                buffer.push_back(ints[i]);
            }

            std::copy(ints, ints+6, vector1.begin());
            std::copy(buffer.begin(), buffer.end(), vector2.begin());

            REQUIRE(vector1 == vector2);
        };

    }
}
