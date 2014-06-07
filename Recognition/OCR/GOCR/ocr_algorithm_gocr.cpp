/**
 * @file ocr_algorithm_gocr.cpp
 * @brief GOCR-based OCR algorithm
 * @author Jason Bunk
 */

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <map>

#include "ocr_algorithm_gocr.hpp"
#include "Recognition/OCR/OCRUtils/ocr_results.hpp"
#include <opencv2/opencv.hpp>
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/SharedUtils_OpenCV.hpp"

extern "C" {
#include "SharedUtils/gocr/gocr.h"
#include "SharedUtils/gocr/pgm2asc.h"
}

using std::cout;
using std::endl;

std::vector<std::pair<char,int>> candidates;
int kMinSetSize = 5;
int kMinGroupSize = 4;
int kGroupDegreeSeparation = 30;

char OCRModuleAlgorithm_GOCR::ProcessCandidates() {

    // Mapping of individual characters to ALL of their angle values
    std::map<char, std::vector<int>> char_map;

    // Mapping of characters to their individual groupings of characters
    // each group of characters is distinct, and is separated by kGroupDegreeSeparation
    // degrees from eachother
    std::map<char, std::vector<std::vector<int>>> group_map;

    //for(std::pair<char, int> candidate_pair : candidates){
    //    cout << candidate_pair.first << "\t|";
    //}
    //cout << endl;
    //for(std::pair<char, int> candidate_pair : candidates){
    //    cout << candidate_pair.second << "\t|";
    //}
    //cout << endl;

    // Sort candidates into lists according to the character
    bool discard_small_sets = false;
    for(std::pair<char, int> cand_pair : candidates) {
        char_map[cand_pair.first].push_back(cand_pair.second);

        // If we ever see a list > the min set size, we want to discard
        // anything < than the set size
        if(!discard_small_sets && char_map[cand_pair.first].size() >= kMinSetSize){
            discard_small_sets = true;
        }
    }

    // Go through the map and discard small sets (if we need to)
    if(discard_small_sets){
        for(std::map<char, std::vector<int>>::iterator it = char_map.begin(); it != char_map.end();){
            if((*it).second.size() < kMinSetSize){
                it = char_map.erase(it);
            }
            else{
                ++it;
            }
        }
    }

    //for(std::pair<char, std::vector<int>> entry : char_map){
    //    cout << entry.first << "\t";
    //    for(int angle : entry.second){
    //        cout << angle << " ";
    //    }
    //    cout << endl;
    //}

    // No entries remain, we don't know the character
    if(char_map.empty()) {
        return '_';
    }

    // Separate each character's degree list into groups
    // Each group is a set of strictly increasing angles with
    // their local change never being more than or equal to kGroupDegreeSeparation
    for(std::pair<char, std::vector<int>> entry : char_map){
        std::vector<std::vector<int>> &group_list = group_map[entry.first];

        std::vector<int> curr_list;
        for(int curr_angle : entry.second){
            // curr_angle is part of this group
            if(curr_list.empty() || abs(curr_list.back() - curr_angle) < kGroupDegreeSeparation){
                curr_list.push_back(curr_angle);
            }
            // curr_angle is not part of this group, new group
            else {
                group_list.push_back(curr_list);
                curr_list.clear();
                curr_list.push_back(curr_angle);
            }
        }

        group_list.push_back(curr_list); // Push back last group

        // 360 -> 0 degree wraparound, so if groups around 0 degrees and groups around 360 degrees could
        // overlap (be part of the same group), move them into same group
        if(group_list.size() > 1 && abs(group_list.front().front() - (group_list.back().back() - 360)) < kGroupDegreeSeparation){
            group_list.front().insert(group_list.front().end(), group_list.back().begin(), group_list.back().end());
            group_list.pop_back();
        }
    }

    // Delete any group with less than kMinGroupSize elements
    for(std::map<char, std::vector<std::vector<int>>>::iterator map_it = group_map.begin(); map_it != group_map.end();){
        std::vector<std::vector<int>> &curr_list = (*map_it).second;
        for(std::vector<std::vector<int>>::iterator vec_it = curr_list.begin(); vec_it != curr_list.end();){
            if((*vec_it).size() < kMinGroupSize){
                vec_it = curr_list.erase(vec_it);
            }
            else{
                ++vec_it;
            }
        }
        if((*map_it).second.size() == 0){
            map_it = group_map.erase(map_it);
        }
        else {
            ++map_it;
        }
    }

    for(std::pair<char, std::vector<std::vector<int>>> group : group_map){
        cout << group.first << ":" << endl;
        for(std::vector<int> list : group.second){
            for(int angle : list){
                cout << angle << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    char retchar;
    int retcharcount = 0;

    // Simple algorithm for deciding retchar: whichever has the most hits wins
    for(std::pair<char, std::vector<std::vector<int>>> group : group_map){
        int currcharcount = 0;
        for(std::vector<int> list : group.second){
            currcharcount += list.size();
        }
        if(currcharcount > retcharcount){
            retchar = group.first;
            retcharcount = currcharcount;
        }
    }

    return retchar;
}

job_t *OCR_JOB;

void OCRModuleAlgorithm_GOCR::RotateAndRunOCR(cv::Mat matsrc, double angle_amount)
{
    cv::Mat mat_rotated, mat_converted, mat_gray, mat_gocr;

    // Rotated input to desired angle, and then convert to binary image (for gocr compatibility)
    Rotate_CV_Mat(matsrc, angle_amount, mat_rotated);
    if(matsrc.channels() >= 3){
        cv::cvtColor(mat_rotated, mat_converted, CV_RGB2GRAY);
    }
    else{
        mat_rotated.copyTo(mat_converted);
    }
    mat_converted.convertTo(mat_gray, CV_8U);
    cv::threshold(mat_gray, mat_gocr, 150, 255, CV_THRESH_BINARY);

    job_t job;
    OCR_JOB = &job;

    // Init properties of job
    job_init(&job);

    // Chars to recognize for this job
    std::string recog_chars("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    char * rec_char_cstr = (char*)malloc(strlen(recog_chars.c_str()) + 1);
    strcpy(rec_char_cstr, recog_chars.c_str());
    job.cfg.lc = rec_char_cstr;

    // Dust size
    job.cfg.dust_size = 20;

    // Certainty
    job.cfg.certainty = certainty_lower_bound;

    // Init various image properties of job
    job_init_image(&job);

    // Our conversion should have taken care of making this a 1 channel, 2-d array
    // If it hasn't, stuff will break below (namely, copying the Mat -> pix)
    if(mat_gocr.channels() != 1){
        cout << "mat_gocr has wrong channel num, something went wrong with conversion" << endl;
    }
    if(mat_gocr.dims != 2){
        cout << "mat_gocr has wrong dim num, something went wrong with conversion" << endl;
    }

    // Put the image into the job
    job.src.p.p = (unsigned char*)malloc(mat_gocr.rows * mat_gocr.cols);
    for(int i = 0; i < mat_gocr.rows * mat_gocr.cols; i++){
        job.src.p.p[i] = mat_gocr.data[i];
    }
    job.src.p.x = mat_gocr.rows;
    job.src.p.y = mat_gocr.cols;
    job.src.p.bpp = 1;

    //cv::Mat testmat(job.src.p.x, job.src.p.y, CV_8U, job.src.p.p);
    //cv::startWindowThread();
    //cv::namedWindow("test", CV_WINDOW_NORMAL);
    //cv::imshow("test", testmat);
    //cv::waitKey(0);

    // Actually run OCR
    pgm2asc(&job);

    std::string gocroutput("");

    // Grab all the OCR output
    int linecounter = 0;
    const char *line;
    line = getTextLine(&(job.res.linelist), linecounter++);
    while (line) {
        gocroutput += line;
        line = getTextLine(&(job.res.linelist), linecounter++);
    }
    free_textlines(&(job.res.linelist));

    //cout << "FOUND GOCR OUTPUT: |" << gocroutput << "|" << endl;

    // Process output
    if(gocroutput.length() <= 2){
        char foundchar = gocroutput.front();
        if(foundchar == '_') {
            foundchar = ' ';
        }
        if(foundchar != ' ' || return_empty_characters) {
            candidates.push_back(std::pair<char, int>(foundchar, angle_amount));
            //last_obtained_results.PushBackNew(certainty_lower_bound+1, angle_amount, foundchar);
        }
    }

    job_free_image(&job);
    free(rec_char_cstr);
}


bool OCRModuleAlgorithm_GOCR::do_OCR(cv::Mat letter_binary_mat) {
    candidates.clear();
    for(int curr_angle = 0.0; curr_angle < 360.0; curr_angle += 360.0 / num_angles_to_check){
        RotateAndRunOCR(letter_binary_mat, curr_angle);
    }
    return (candidates.empty() == false);
}
