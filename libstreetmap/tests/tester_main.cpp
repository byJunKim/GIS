/* 
 * Copyright 2018 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <iostream>
#include <unittest++/UnitTest++.h>
#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "m4.h"
#include "m4.cpp"
#include "m2.cpp"
#include <string>
#include "StreetsDatabaseAPI.h"
#include "map_data.h"
#include "path_verify.h"
#include "courier_verify.h"
#include "courier_verify.cpp"

/*
 * This is the main that drives running
 * unit tests.
 */

//std::vector<std::string> streetNames;

int main(int /*argc*/, char** /*argv*/) {
    //Run the unit tests

    //loading the map of toronto
    std::string default_map_path = "/cad2/ece297s/public/maps/toronto_canada.streets.bin";

    load_map(default_map_path);

    //data structure: a vector where each index is street id and each element
    //has a street name
    //        streetNames.resize(getNumberOfStreets());
    //
    //        for (unsigned i = 0; i < getNumberOfStreets(); i++) {
    //            streetNames[i] = getStreetName(i);
    //
    //        }

    int num_failures = UnitTest::RunAllTests();
    return num_failures;

}

//}
//
////test cases are from the exerciser
//
//TEST(are_directly_connected) {
//
//    CHECK_EQUAL(0, are_directly_connected(7339, 68888));
//    CHECK_EQUAL(1, are_directly_connected(19446, 82786));
//}
//
//
////test cases are from the exercier
//
//TEST(find_street_segment_length) {
//
//    double expected = 530.6419818323895;
//    double actual = find_street_segment_length(89824);
//    CHECK(abs(expected - actual) < 0.001);
//
//}
//
////checks if it returns an empty vector for a random street name that does not exist.
////using find_street_ids_from_name function, get a vector of street ids that have
////the same street name
////and get the street name from the ids using API function
////compare the expected street name from the data structure and the street name
////from the function
//
//TEST(find_street_ids_from_name) {
//    UNITTEST_TIME_CONSTRAINT(250);
//    std::vector<unsigned> actualIDs;
//    std::string actualName;
//
//    //giving it a random street name
//    //check if it returns an empty vector
//    std::string randomName = "TEAM039 Street";
//    actualIDs = find_street_ids_from_name(randomName);
//    CHECK(actualIDs.size() == 0);
//
//    for (unsigned i = 0; i < getNumberOfStreets(); i++) {
//        //get street ids that have the same name using the function
//        actualIDs = find_street_ids_from_name(streetNames[i]);
//        for (unsigned j = 0; j < actualIDs.size(); j++) {
//            //read out the street name corresponding to the id, using API function
//            actualName = getStreetName(actualIDs[j]);
//            //Check if actual == expectedName (from data structure)
//            CHECK(actualName == streetNames[i]);
//        }
//    }
//}
//
////Test the speed of the function
////checks if 2 million of the function call takes less than 250 ms
//
//TEST(time) {
//
//    UNITTEST_TIME_CONSTRAINT(250);
//
//    std::vector<unsigned> actualIDs;
//    std::string actualName;
//
//
//    for (unsigned i = 0; i < 2000000; i++) {
//        //get street ids that have the same name using the function
//        actualIDs = find_street_ids_from_name("Barberry Lane");
//    }
//}

//
//TEST(draw_features){
////    UNITTEST_TIME_CONSTRAINT(500);
//    
//    draw_features(0);
//    
//}

TEST(m3){
    std::vector<DeliveryInfo> deliveries;
    std::vector<unsigned> depots;
    float turn_penalty;
    std::vector<unsigned> result_path;

//    deliveries = {DeliveryInfo(73593, 104453), DeliveryInfo(1981, 47060), DeliveryInfo(58204, 31403), DeliveryInfo(102129, 63975), DeliveryInfo(41153, 63608)};
//    depots = {2521, 5153, 41138};
//    turn_penalty = 15;
//    result_path = traveling_courier(deliveries, depots, turn_penalty);
    
    deliveries = {DeliveryInfo(64685, 77941), DeliveryInfo(101590, 3242), DeliveryInfo(2550, 75097), DeliveryInfo(100485, 23837), DeliveryInfo(87422, 61041), DeliveryInfo(107789, 86093), DeliveryInfo(3260, 25407), DeliveryInfo(33310, 15602), DeliveryInfo(67398, 21673), DeliveryInfo(22066, 59404), DeliveryInfo(86654, 41771), DeliveryInfo(20012, 20118), DeliveryInfo(101883, 41179), DeliveryInfo(41299, 107201), DeliveryInfo(13148, 88578), DeliveryInfo(49250, 20385), DeliveryInfo(35861, 48683), DeliveryInfo(19496, 64015), DeliveryInfo(92759, 94111), DeliveryInfo(85261, 63091), DeliveryInfo(82000, 67157), DeliveryInfo(53017, 90276), DeliveryInfo(17903, 94979), DeliveryInfo(86978, 18281), DeliveryInfo(57239, 39482)};
    depots = {4, 18513};
    turn_penalty = 15;
    {

            result_path = traveling_courier(deliveries, depots, turn_penalty);
    }
}



//
//#include <random>
//#include <unittest++/UnitTest++.h>
//
//#include "StreetsDatabaseAPI.h"
//#include "m1.h"
//#include "m3.h"
//
//#include "unit_test_util.h"
//#include "path_verify.h"
//
//using ece297test::relative_error;
//using ece297test::path_is_legal;
//
//
//SUITE(inter_inter_simple_path_func_public) {
//struct MapFixture {
//    MapFixture() {
//        rng = std::minstd_rand(4);
//        rand_intersection = std::uniform_int_distribution<unsigned>(0, getNumberOfIntersections()-1);
//        rand_street = std::uniform_int_distribution<unsigned>(1, getNumberOfStreets()-1);
//        rand_segment = std::uniform_int_distribution<unsigned>(0, getNumberOfStreetSegments()-1);
//        rand_poi = std::uniform_int_distribution<unsigned>(0, getNumberOfPointsOfInterest()-1);
//        rand_lat = std::uniform_real_distribution<double>(46.650653839, 46.730262756);
//        rand_lon = std::uniform_real_distribution<double>(7.667298794, 7.962547302);
//        rand_turn_penalty = std::uniform_real_distribution<double>(0., 30.);
//    }
//
//    std::minstd_rand rng;
//    std::uniform_int_distribution<unsigned> rand_intersection;
//    std::uniform_int_distribution<unsigned> rand_street;
//    std::uniform_int_distribution<unsigned> rand_segment;
//    std::uniform_int_distribution<unsigned> rand_poi;
//    std::uniform_real_distribution<double> rand_lat;
//    std::uniform_real_distribution<double> rand_lon;
//    std::uniform_real_distribution<double> rand_turn_penalty;
//};
//    TEST(find_path_between_intersections_simple_legality) {
//        std::vector<unsigned> path;
//
//        path = find_path_between_intersections(103, 104, 0.00000000000000000);
//        //CHECK(path_is_legal(103, 104, path));
//
//        path = find_path_between_intersections(103, 54, 0.00000000000000000);
//        //CHECK(path_is_legal(103, 54, path));
//
//        path = find_path_between_intersections(141, 297, 0.00000000000000000);
//        //CHECK(path_is_legal(141, 297, path));
//
//        path = find_path_between_intersections(418, 419, 0.00000000000000000);
//        //CHECK(path_is_legal(418, 419, path));
//
//        path = find_path_between_intersections(607, 606, 0.00000000000000000);
//        //CHECK(path_is_legal(607, 606, path));
//
//        path = find_path_between_intersections(644, 645, 0.00000000000000000);
//        //CHECK(path_is_legal(644, 645, path));
//
//        path = find_path_between_intersections(658, 665, 0.00000000000000000);
//        //CHECK(path_is_legal(658, 665, path));
//
//        path = find_path_between_intersections(702, 703, 0.00000000000000000);
//        //CHECK(path_is_legal(702, 703, path));
//
//        path = find_path_between_intersections(724, 499, 0.00000000000000000);
//        //CHECK(path_is_legal(724, 499, path));
//
//        path = find_path_between_intersections(496, 725, 0.00000000000000000);
//        //CHECK(path_is_legal(496, 725, path));
//
//        path = find_path_between_intersections(748, 564, 0.00000000000000000);
//        //CHECK(path_is_legal(748, 564, path));
//                                                                                                                                                                                                                           1,2           Top
//    }
//}

//
//SUITE(simple_legality_toronto_canada_public) {
//    TEST(simple_legality_toronto_canada) {
//        std::vector<DeliveryInfo> deliveries;
//        std::vector<unsigned> depots;
//        std::vector<unsigned> result_path;
//        float turn_penalty;
//        /*
//        deraveling_courier(deliveries, depots, turn_penalty);liveries = {DeliveryInfo(73593, 104453), DeliveryInfo(1981, 47060), DeliveryInfo(58204, 31403), DeliveryInfo(102129, 63975), DeliveryInfo(41153, 63608)};
//        depots = {2521, 5153, 41138};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        */
//        /*
//        deliveries = {DeliveryInfo(102343, 40664), DeliveryInfo(66501, 33150), DeliveryInfo(64220, 70192), DeliveryInfo(10143, 89556), DeliveryInfo(51798, 85214)};
//        depots = {14526, 21436, 41416};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//       */ 
//        /*
//        deliveries = {DeliveryInfo(9256, 65463)};
//        depots = {2};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        
//        
//        */
//        /*
//        deliveries = {DeliveryInfo(9140, 19741), DeliveryInfo(67133, 63045)};
//        depots = {24341, 84950};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        */
//        /*
//        deliveries = {DeliveryInfo(7552, 11395), DeliveryInfo(20877, 76067), DeliveryInfo(76915, 76067), DeliveryInfo(108204, 76067), DeliveryInfo(108204, 99189), DeliveryInfo(32523, 99189), DeliveryInfo(32523, 92242), DeliveryInfo(32523, 99189), DeliveryInfo(32523, 52119)};
//        depots = {32736, 27838, 28149};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        
//        */
//        /*
//       deliveries = {DeliveryInfo(82141, 93835), DeliveryInfo(72558, 3972), DeliveryInfo(56282, 90416), DeliveryInfo(107044, 44237), DeliveryInfo(67389, 44237), DeliveryInfo(80027, 98370), DeliveryInfo(99714, 93835), DeliveryInfo(20449, 93835)};
//        depots = {33451, 24583, 88889};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//       */  
//        
//        /*
//       deliveries = {DeliveryInfo(43046, 85976)};
//        depots = {33578, 56051};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//         */
//        
//        /*
//       deliveries = {DeliveryInfo(51656, 73523), DeliveryInfo(13931, 48963), DeliveryInfo(59889, 44527), DeliveryInfo(43679, 67308), DeliveryInfo(22657, 81046)};
//        depots = {36090, 38968, 33098};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//         */
//        
//        /*
//       deliveries = {DeliveryInfo(42364, 28031), DeliveryInfo(47309, 59485), DeliveryInfo(19950, 83619), DeliveryInfo(19950, 28031), DeliveryInfo(15381, 59485), DeliveryInfo(42364, 59485), DeliveryInfo(19950, 66464), DeliveryInfo(89161, 21391), DeliveryInfo(19950, 28031)};
//        depots = {42952, 27383, 50571};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//      */   
//       /*
//        deliveries = {DeliveryInfo(18320, 85366), DeliveryInfo(59662, 97286), DeliveryInfo(98769, 79154), DeliveryInfo(81171, 35000), DeliveryInfo(89377, 15810)};
//        depots = {53592, 35108, 73562};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        */
//       
//        /*
//        deliveries = {DeliveryInfo(43327, 28619)};
//        depots = {56060};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        */
//        deliveries = {DeliveryInfo(103810, 3723), DeliveryInfo(34578, 90920), DeliveryInfo(76994, 35921), DeliveryInfo(47909, 90920), DeliveryInfo(80021, 39409), DeliveryInfo(106449, 35921), DeliveryInfo(22113, 35921), DeliveryInfo(5385, 95758)};
//        depots = {58218, 19461, 56760};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        
//        
//        /*
//         *
//       
//        
//        deliveries = {DeliveryInfo(52747, 24225), DeliveryInfo(52747, 22444), DeliveryInfo(12519, 65813), DeliveryInfo(52747, 62211), DeliveryInfo(26565, 72357), DeliveryInfo(18521, 75984), DeliveryInfo(80021, 789), DeliveryInfo(80021, 50674)};
//        depots = {58853, 4637, 59870};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(64489, 10192), DeliveryInfo(66348, 47055)};
//        depots = {75020, 59249};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(7261, 90648), DeliveryInfo(42431, 20777), DeliveryInfo(18005, 104347)};
//        depots = {63031, 73383, 10168};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(55705, 95430), DeliveryInfo(108325, 79055)};
//        depots = {64436};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(53600, 84132), DeliveryInfo(7801, 20885)};
//        depots = {65267, 91978};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(14020, 18998), DeliveryInfo(48091, 4060), DeliveryInfo(54883, 50882)};
//        depots = {36451, 49616, 8297};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(23069, 80185), DeliveryInfo(23459, 60949), DeliveryInfo(62940, 18214), DeliveryInfo(81951, 19441), DeliveryInfo(86025, 46362)};
//        depots = {80138, 25819, 5507};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(9748, 61004), DeliveryInfo(63381, 88129)};
//        depots = {80938};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(90183, 84955), DeliveryInfo(57973, 86285), DeliveryInfo(57973, 63719), DeliveryInfo(99338, 85306), DeliveryInfo(1024, 16646), DeliveryInfo(86476, 62274), DeliveryInfo(57973, 103682), DeliveryInfo(90183, 16062)};
//        depots = {81982, 75396, 34731};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(44848, 98941), DeliveryInfo(108022, 62818), DeliveryInfo(17844, 76041)};
//        depots = {80481, 34196, 47772};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(15508, 102530), DeliveryInfo(91823, 22518), DeliveryInfo(104224, 42933), DeliveryInfo(91823, 102530), DeliveryInfo(79815, 102530), DeliveryInfo(76026, 19841), DeliveryInfo(76026, 22518), DeliveryInfo(76026, 19489), DeliveryInfo(76026, 22518)};
//        depots = {27131, 69454, 50227};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        
//        
//        deliveries = {DeliveryInfo(17930, 93400)};
//        depots = {98141, 104672};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(101919, 45138)};
//        depots = {98719, 32007};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(25692, 87297), DeliveryInfo(33040, 6927), DeliveryInfo(104990, 106963), DeliveryInfo(99873, 6118), DeliveryInfo(24190, 106963), DeliveryInfo(66817, 6927), DeliveryInfo(48830, 6927), DeliveryInfo(47500, 107097)};
//        depots = {99219, 33748, 86378};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(26409, 44178), DeliveryInfo(71224, 80079), DeliveryInfo(102350, 45533), DeliveryInfo(64908, 100186), DeliveryInfo(98394, 12677)};
//        depots = {104947, 105823, 71751};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(32342, 46379), DeliveryInfo(97919, 71085)};
//        depots = {105225};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        
//        deliveries = {DeliveryInfo(29579, 21548), DeliveryInfo(73225, 16427), DeliveryInfo(73225, 27760), DeliveryInfo(67738, 66845), DeliveryInfo(67738, 63977), DeliveryInfo(73225, 86972), DeliveryInfo(64559, 97609), DeliveryInfo(29444, 43148)};
//        depots = {107859, 12362, 94777};
//        turn_penalty = 15;
//        result_path = traveling_courier(deliveries, depots, turn_penalty);
//        CHECK(courier_path_is_legal(deliveries, depots, result_path));
//        */
//    } //simple_legality_toronto_canada
//
//} //simple_legality_toronto_canada_public

TEST(TestDrawToronto) {
    std::vector<DeliveryInfo> deliveries;
    std::vector<unsigned> depots;
    float turn_penalty;
    std::vector<unsigned> result_path;

    deliveries = {DeliveryInfo(70831, 51733), DeliveryInfo(64614, 49104), DeliveryInfo(86395, 90198), DeliveryInfo(68915, 1748), DeliveryInfo(77817, 36464), DeliveryInfo(7906, 86992), DeliveryInfo(85195, 97964), DeliveryInfo(24366, 11113), DeliveryInfo(62657, 72429), DeliveryInfo(44826, 33045), DeliveryInfo(6566, 86491), DeliveryInfo(91850, 60035), DeliveryInfo(36280, 57275), DeliveryInfo(97481, 1195), DeliveryInfo(68300, 60860), DeliveryInfo(77791, 15128), DeliveryInfo(69739, 8078), DeliveryInfo(68385, 108770), DeliveryInfo(75459, 100013), DeliveryInfo(67285, 100826), DeliveryInfo(16103, 47349), DeliveryInfo(44690, 30333), DeliveryInfo(989, 81045), DeliveryInfo(95265, 10843), DeliveryInfo(15804, 90041), DeliveryInfo(66262, 30923), DeliveryInfo(69352, 11124), DeliveryInfo(39741, 80709), DeliveryInfo(107067, 9155), DeliveryInfo(9727, 87561), DeliveryInfo(14679, 69801), DeliveryInfo(49350, 45035), DeliveryInfo(46806, 39996), DeliveryInfo(22340, 50841), DeliveryInfo(83735, 29110), DeliveryInfo(84125, 35208), DeliveryInfo(27827, 94577), DeliveryInfo(99880, 45391), DeliveryInfo(51371, 56818), DeliveryInfo(108031, 19488), DeliveryInfo(41567, 48318), DeliveryInfo(2266, 5702), DeliveryInfo(82273, 58171), DeliveryInfo(105662, 92219), DeliveryInfo(71511, 93073), DeliveryInfo(76398, 49443), DeliveryInfo(44689, 105495), DeliveryInfo(20806, 49666), DeliveryInfo(74292, 43619), DeliveryInfo(99932, 46063), DeliveryInfo(67216, 82420), DeliveryInfo(83186, 45317), DeliveryInfo(76221, 39119), DeliveryInfo(23822, 101787), DeliveryInfo(30252, 47210), DeliveryInfo(21261, 30777), DeliveryInfo(32640, 60454), DeliveryInfo(5132, 5321), DeliveryInfo(36493, 6120), DeliveryInfo(57286, 100769), DeliveryInfo(96376, 26250), DeliveryInfo(51728, 103554), DeliveryInfo(40558, 39484), DeliveryInfo(2725, 55441), DeliveryInfo(74490, 26052), DeliveryInfo(39219, 29203), DeliveryInfo(81784, 3342), DeliveryInfo(9007, 54902), DeliveryInfo(74175, 27755), DeliveryInfo(21235, 22648), DeliveryInfo(53109, 15924), DeliveryInfo(4863, 44723), DeliveryInfo(37191, 76062), DeliveryInfo(88341, 85383), DeliveryInfo(94683, 55071), DeliveryInfo(36870, 22371), DeliveryInfo(27010, 53986), DeliveryInfo(4562, 6641), DeliveryInfo(54939, 83587), DeliveryInfo(56452, 69532), DeliveryInfo(98765, 72206), DeliveryInfo(57705, 18710), DeliveryInfo(93630, 76760), DeliveryInfo(75607, 23835), DeliveryInfo(50531, 62871), DeliveryInfo(87576, 103929), DeliveryInfo(75119, 104926), DeliveryInfo(28917, 94863), DeliveryInfo(48014, 79421), DeliveryInfo(67807, 70881), DeliveryInfo(36713, 40596), DeliveryInfo(60356, 107976), DeliveryInfo(61155, 49594), DeliveryInfo(5404, 68375), DeliveryInfo(86324, 87156), DeliveryInfo(58034, 43156), DeliveryInfo(26541, 93050), DeliveryInfo(49782, 56269), DeliveryInfo(98805, 88285), DeliveryInfo(96318, 57406), DeliveryInfo(3909, 1063), DeliveryInfo(106669, 38602), DeliveryInfo(60559, 39417), DeliveryInfo(32310, 77756), DeliveryInfo(43215, 79559), DeliveryInfo(2719, 85706), DeliveryInfo(44546, 50836), DeliveryInfo(103884, 51061), DeliveryInfo(33352, 97894), DeliveryInfo(13982, 53927), DeliveryInfo(39136, 83642), DeliveryInfo(16454, 16407), DeliveryInfo(107566, 10186), DeliveryInfo(55738, 29559), DeliveryInfo(20433, 64640), DeliveryInfo(51018, 47704), DeliveryInfo(83418, 20963), DeliveryInfo(46690, 88588), DeliveryInfo(79849, 202), DeliveryInfo(74264, 55481), DeliveryInfo(86580, 46058), DeliveryInfo(41171, 66137), DeliveryInfo(75263, 31994), DeliveryInfo(9586, 45923), DeliveryInfo(103834, 69443), DeliveryInfo(50331, 53154), DeliveryInfo(60773, 99528), DeliveryInfo(55406, 19758), DeliveryInfo(6145, 89729), DeliveryInfo(108448, 58740), DeliveryInfo(81702, 13838), DeliveryInfo(103362, 31397), DeliveryInfo(54844, 92286), DeliveryInfo(43567, 69760), DeliveryInfo(79978, 86281), DeliveryInfo(22727, 51589), DeliveryInfo(80415, 31898), DeliveryInfo(94398, 36759), DeliveryInfo(85643, 11886), DeliveryInfo(99530, 91160), DeliveryInfo(24362, 91302), DeliveryInfo(61012, 103837), DeliveryInfo(65021, 32757), DeliveryInfo(51692, 101825), DeliveryInfo(98287, 68477), DeliveryInfo(61805, 90143), DeliveryInfo(92611, 90642), DeliveryInfo(89383, 26267), DeliveryInfo(107584, 16088), DeliveryInfo(26525, 77272), DeliveryInfo(82828, 107683), DeliveryInfo(47068, 107059), DeliveryInfo(41357, 87541), DeliveryInfo(1224, 69355), DeliveryInfo(25809, 18541), DeliveryInfo(98564, 427), DeliveryInfo(68616, 101140), DeliveryInfo(41462, 75845), DeliveryInfo(84210, 94998), DeliveryInfo(47770, 5933), DeliveryInfo(74323, 20719), DeliveryInfo(96722, 88605), DeliveryInfo(98608, 101900), DeliveryInfo(16936, 27083), DeliveryInfo(47448, 39881), DeliveryInfo(74333, 43454), DeliveryInfo(18735, 66274), DeliveryInfo(51610, 59328), DeliveryInfo(7572, 44358), DeliveryInfo(96967, 76061), DeliveryInfo(20466, 30814), DeliveryInfo(78564, 5062), DeliveryInfo(102853, 7788), DeliveryInfo(47378, 8940), DeliveryInfo(4738, 45674)};
    depots = {14, 55539, 66199, 38064, 87930, 15037, 9781, 42243, 62859, 50192, 58489, 39444, 53709, 93336, 60037, 7650, 28282, 105649, 38892, 87051};
    turn_penalty = 15;
    {
        result_path = traveling_courier(deliveries, depots, turn_penalty);
    }
    
        
         deliveries = {DeliveryInfo(55705, 95430) /*, DeliveryInfo(108325, 79055) */};
        depots = {64436};
        turn_penalty = 15;
        result_path = traveling_courier(deliveries, depots, turn_penalty);
    
    
     deliveries = {DeliveryInfo(52747, 24225), DeliveryInfo(52747, 22444), DeliveryInfo(12519, 65813), DeliveryInfo(52747, 62211), DeliveryInfo(26565, 72357), DeliveryInfo(18521, 75984), DeliveryInfo(80021, 789), DeliveryInfo(80021, 50674)};
        depots = {58853, 4637, 59870};
        turn_penalty = 15;
        result_path = traveling_courier(deliveries, depots, turn_penalty);
        {
        result_path = traveling_courier(deliveries, depots, turn_penalty);
    }
        
        deliveries = {DeliveryInfo(105371, 20649)};
        depots = {97060};
        turn_penalty = 15;
        result_path = traveling_courier(deliveries, depots, turn_penalty);
        
        deliveries = {DeliveryInfo(29579, 21548), DeliveryInfo(73225, 16427), DeliveryInfo(73225, 27760), DeliveryInfo(67738, 66845), DeliveryInfo(67738, 63977), DeliveryInfo(73225, 86972), DeliveryInfo(64559, 97609), DeliveryInfo(29444, 43148)};
        depots = {107859, 12362, 94777};
        turn_penalty = 15;
        result_path = traveling_courier(deliveries, depots, turn_penalty);
        
        deliveries = {DeliveryInfo(29579, 21548), DeliveryInfo(73225, 16427), DeliveryInfo(73225, 27760), DeliveryInfo(67738, 66845), DeliveryInfo(67738, 63977), DeliveryInfo(73225, 86972), DeliveryInfo(64559, 97609), DeliveryInfo(29444, 43148)};
        depots = {107859, 12362, 94777};
        turn_penalty = 15;
        result_path = traveling_courier(deliveries, depots, turn_penalty);

    //assume its legal
    double path_cost = compute_path_travel_time(result_path, turn_penalty);
    std::cout << "QoR extreme_toronto_canada: " << path_cost << std::endl;

    mapData->deliveriesTemp = deliveries;
    mapData->depotsTemp = depots;
    mapData -> pathTemp = result_path;

    
    draw_map();

//    close_map();
}
