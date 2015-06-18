#include "Georeference.hpp"

// Calculate pixels per feet (resolution) of given image
// accounting for image resize scalefactor, if it was resized
static double static_calculate_px_per_feet(double horiz_cols, double altitude, double scalefactor)
{
    double focal_length = 35, crop_factor = 1.6; 
    double equiv_foc_len = focal_length * crop_factor;

    double h_fov = 2 * atan(36/(2*equiv_foc_len)); 

    double h_ground = 2 * altitude * tan(h_fov/2);
    double px_per_feet = horiz_cols/h_ground;

    return px_per_feet * scalefactor;
}

std::pair<double, double> georeference_target_in_image(int targetrow,
                                                       int targetcol,
                                                       int imrows,
                                                       int imcols,
                                                       double planelat,
                                                       double planelongt,
                                                       double planeheading,
                                                       double planealtitude)
{
    double pxperfeet = static_calculate_px_per_feet(imcols, planealtitude, 1.0); //default scalefactor: 1.0
    
    planeheading = planeheading - kPI; //Gimbal is reversed in plane

    double rowdiff = targetrow - (double)imrows/2; //row diff from center (and center of plane)
    double coldiff = targetcol - (double)imcols/2; //col diff from center (and center of plane)

    // Tranlate to polar coordinates, in feet
    double centerdiff = sqrt(pow(rowdiff, 2) + pow(coldiff, 2));
    double centerfeetdiff = centerdiff / pxperfeet;
    double centerangle = atan2(coldiff, rowdiff); //clockwise from up (we use coordinate system North-Is-Up == 0 degrees, East-Is-Right == 90 degrees)
                                                  //the image may not be facing up==north, so planeheading is used as the offset
                                                  //atan2(x,y) is clockwise from up
                                                  //atan2(y,x) is counterclockwise from right (ordinary polar coordinates)
    
    // Project to Lat/Long; convert planeheading from degrees to radians
    double latfeetdiff = centerfeetdiff * cos(planeheading*0.01745329251994329577 + centerangle);
    double longtfeetdiff = centerfeetdiff * sin(planeheading*0.01745329251994329577 + centerangle);

    // Convert Lat/Long feet differences into degrees to get final lat/long
    double target_lat = planelat + latfeetdiff/365221.43; //365221 feet in 1 degree of latitude arc, small angle assumptions for field; 
    double longt_deg_to_feet = kPI * 20898855.01138578 * cos(to_radians(target_lat)) / 180; //Radius of circle at this lat, (PI*R)/(180)
    double target_longt = planelongt + longtfeetdiff/longt_deg_to_feet;
    
    //cout << "heading: " << planeheading << endl;
    //cout << "imrows: " << imrows << " imcols: " << imcols << endl;
    //cout << "rd: " << rowdiff << endl;
    //cout << "cd: " << coldiff << endl;
    //cout << "ppf: " << pxperfeet << endl;
    //cout << "centerdiff: " << centerdiff << endl;
    //cout << "centerfeetdiff: " << centerfeetdiff << endl;
    //cout << "centerangle: " << centerangle << endl;
    //printf("lat diff: %.7f\tdeg, %.3f feet\n", latfeetdiff/365221, latfeetdiff);
    //printf("long diff: %.7f\tdeg, %.3f feet\n", longtfeetdiff/longt_deg_to_feet, longtfeetdiff);
    
    return std::pair<double, double>(target_lat, target_longt);
}
