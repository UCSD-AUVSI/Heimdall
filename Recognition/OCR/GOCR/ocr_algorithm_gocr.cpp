/**
 * @file ocr_algorithm_gocr.cpp
 * @brief GOCR-based OCR algorithm
 * @author Jason Bunk
 */

#include <iostream>
#include <cstdlib>

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

bool OCRModuleAlgorithm_GOCR::TryToInitializeMe()
{
    //assume it's there
    return true;
}


void OCRModuleAlgorithm_GOCR::do_SiftThroughCandidates(OCR_ResultsContainer & given_results,
        int max_num_reported_letters,
        double cutoff_confidence)
{
    //this method assumes that each letter has an attached confidence value
    //if the OCR algorithm doesn't return confidence values, some other means of sorting must be used

    std::map<char, int> char_appearances;
    std::map<char, double> char_std_deviances;
    //std::map<char, double> char_std_deviances__doublebinned;
    std::map<char, double> char_confidences;
    std::map<char, double> char_angles;

    std::vector<OCR_Result>::iterator riter = given_results.results.begin();
    for(; riter != given_results.results.end(); riter++) {
        if(riter->character != ' ') {
            if(char_appearances.find(riter->character) == char_appearances.end()) {
                char_appearances[riter->character] = 0;

                if(letter_has_only_one_orientation(riter->character)) {
                    double letter_standard_deviance = 0.0;
                    char_angles[riter->character] = OCR_ResultsContainer::GetMeanAngle_FromOCRResults(given_results.results, riter->character, &letter_standard_deviance);
                    char_std_deviances[riter->character] = letter_standard_deviance;
                }
                else if(letter_has_two_orientations(riter->character)) {
                    char_std_deviances[riter->character] = 0.5 * OCR_ResultsContainer::Get_DoubleBinned_StdDev_FromOCRResults(given_results.results, riter->character);
                }
                else {
                    //what to do here? don't bother calculating?
                    char_std_deviances[riter->character] = 1.0;
                }
            }
            char_appearances[riter->character]++;
        }
    }


    given_results.clear();


    std::map<char, int>::iterator miter = char_appearances.begin();
    for(; miter != char_appearances.end(); miter++)
    {
        if(char_appearances[miter->first] >= 5)
        {
            double its_num_appearances = static_cast<double>(char_appearances[miter->first]);
            char_confidences[miter->first] = (pow(its_num_appearances, 1.0) / pow(char_std_deviances[miter->first], 1.0));
        }
        else
        {
            char_confidences[miter->first] = 0.0;
        }

        given_results.PushBackNew(char_confidences[miter->first], char_angles[miter->first], miter->first);
    }


    given_results.SortByConfidence();


    std::map<char, int> new_chars____test_for_printing;
    riter = given_results.results.begin();
    for(; riter != given_results.results.end(); riter++)
    {
        if(riter->character != ' ')
        {
            if(new_chars____test_for_printing.find(riter->character) == new_chars____test_for_printing.end())
            {
                new_chars____test_for_printing[riter->character] = 0;
                consoleOutput.Level2() << "letter: " << riter->character << ", num appearances: " << char_appearances[riter->character] << ", std dev: " << char_std_deviances[riter->character] << ", \"conf\": " << char_confidences[riter->character] << std::endl;
            }
        }
    }
}

job_t *OCR_JOB;

void OCRModuleAlgorithm_GOCR::RotateAndRunOCR(cv::Mat matsrc, double angle_amount, bool return_empty_characters)
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
            last_obtained_results.PushBackNew(certainty_lower_bound+1, angle_amount, foundchar);
        }
    }

    job_free_image(&job);
    free(rec_char_cstr);
}


bool OCRModuleAlgorithm_GOCR::do_OCR(cv::Mat letter_binary_mat, std::ostream* PRINTHERE/*=nullptr*/, bool return_empty_characters/*=false*/)
{
    last_obtained_results.clear();
    cout << " HERE " << endl;
    for(int curr_angle = 0.0; curr_angle < 360.0; curr_angle += 360.0 / num_angles_to_check){
        RotateAndRunOCR(letter_binary_mat, curr_angle, return_empty_characters);
    }

    ////rotate a bunch and detect chars
    //double angle_current=0.0;
    //double angle_delta = (360.0 / static_cast<double>(num_angles_to_check));
    //for(int bb=0; bb<num_angles_to_check; bb++)
    //{
    //    RotateAndRunOCR(letter_binary_mat, angle_current, return_empty_characters);
    //    angle_current += angle_delta;
    //}

    return (last_obtained_results.empty() == false);
}
