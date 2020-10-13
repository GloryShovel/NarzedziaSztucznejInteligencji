#include <cv.hpp>
#include <highgui.h>
#include <iostream>

int main( int argc, char** argv ) {
    bool capturing = true;
    // Creating cap with reference to avi file
    cv::VideoCapture cap( "drop.avi" );
    // cv::VideoCapture cap(0);
    if ( !cap.isOpened() ) {
        std::cerr << "error opening frames source" << std::endl;
        return -1;
    }
    std::cout << "Video size: " << cap.get( cv::CAP_PROP_FRAME_WIDTH )
    << "x" << cap.get( cv::CAP_PROP_FRAME_HEIGHT ) << std::endl;
    do {
        cv::Mat frame;
        if ( cap.read( frame ) ) {
          cv::flip(frame, frame, 1);
            cv::imshow( "Not-yet smart windown", frame );
        } else {
            // stream finished
            capturing = false;
        }
        //czekaj na klawisz, sprawdz czy to jest 'esc'
        if( (cv::waitKey( 1000.0/60.0 )&0x0ff) == 27 ) capturing = false;
    } while( capturing );
    return 0;
}
