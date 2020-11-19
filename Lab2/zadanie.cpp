#include <cv.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>


int main( int argc, char** argv ) {

    bool capturing = true;
    //all frames
    cv::Mat frame, bluredFrame, resizedFrame, hsvFrame, hsvInRangeFrame;
    //for sliders
    const int sliderMax = 255;
    int colorSliderMin = 0, colorSliderMax = 255, saturationSliderMin = 0, saturationSliderMax = 255, valueSliderMin = 0, valueSliderMax = 255;

    // Creating cap with reference to avi file
    cv::VideoCapture cap( "drop.avi" );
    if ( !cap.isOpened() ) {
        std::cerr << "error opening frames source" << std::endl;
        return -1;
    }

    std::cout << "Video size: " << cap.get( cv::CAP_PROP_FRAME_WIDTH )
    << "x" << cap.get( cv::CAP_PROP_FRAME_HEIGHT ) << std::endl;




    do {

      if(!cap.read(frame)){
        std::cout << "Error capturing frame" << std::endl;
        break;
        //TODO: loop avi file
      }

      //Loops the video
      if( cap.get(cv::CAP_PROP_POS_FRAMES) == cap.get(cv::CAP_PROP_FRAME_COUNT)){
      cap.set(cv::CAP_PROP_POS_FRAMES, 1);
      }

      //make Gaussian blur and show processed image
      cv::GaussianBlur(frame, bluredFrame, cv::Size(5,5), 0);

      //Check for comand flags (first is x second is y)
      if(argc >= 3){
        cv::resize(frame, resizedFrame, cv::Size(atoi(argv[1]), atoi(argv[2])));
      }else{
        cv::resize(frame, resizedFrame, cv::Size(320,200));
      }

      //Changing the color space
      cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);

      //TODO: chceck why resize converts image back to BGR color space
      // cv::resize(frame, hsvFrame, cv::Size(600,480));

      //Making adjustable inRange frame
      cv::createTrackbar("colorSliderMin", "Inrange", &colorSliderMin, sliderMax);
      cv::createTrackbar("colorSliderMax", "Inrange", &colorSliderMax, sliderMax);
      cv::createTrackbar("saturationSliderMin", "Inrange", &saturationSliderMin, sliderMax);
      cv::createTrackbar("saturationSliderMax", "Inrange", &saturationSliderMax, sliderMax);
      cv::createTrackbar("valueSliderMin", "Inrange", &valueSliderMin, sliderMax);
      cv::createTrackbar("valueSliderMax", "Inrange", &valueSliderMax, sliderMax);
      cv::inRange(hsvFrame, cv::Scalar(colorSliderMin, saturationSliderMin, valueSliderMin), cv::Scalar(colorSliderMax, saturationSliderMax, valueSliderMax), hsvInRangeFrame);

      cv::putText(hsvFrame, "ColorMIN:"+std::to_string(colorSliderMin)+" ColorMAX:"+std::to_string(colorSliderMax), cv::Point(5,11), cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(0,0,0), 2);
      cv::putText(hsvFrame, "SaturationMIN:"+std::to_string(saturationSliderMin)+" SaturationMAX:"+std::to_string(saturationSliderMax), cv::Point(5,22), cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(0,0,0), 2);
      cv::putText(hsvFrame, "ValueMIN:"+std::to_string(valueSliderMin)+" ValueMAX:"+std::to_string(valueSliderMax), cv::Point(5,33), cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(0,0,0), 2);

      //DO NOT TOUCH THE KEY!!!!
      //This value is not something that i calculated, but its frame rate dependent
      int key = cv::waitKey(1000.0/60.0 )&0x0ff;
      if(key == 'x' ){
        //Saving ROI (selectOIR doesn't work on my linux can't figure it out, but this is what it should look like)
        cv::Rect roi = cv::selectROI("Basic",frame);
        // cv::Rect roi;
        // roi.x = 0;
        // roi.y = 0;
        // roi.width = 200;
        // roi.height = 200;
        cv::Mat imCrop = frame(roi);
        cv::imwrite("screen_shot.png", imCrop);
      }

      //TODO: make all frames visible at diffrent locations
      cv::imshow("Basic", frame);
      cv::imshow("Blured", bluredFrame);
      cv::imshow("Resized", resizedFrame);
      cv::imshow("HSV", hsvFrame);
      cv::imshow("Inrange", hsvInRangeFrame);

        //czekaj na klawisz, sprawdz czy to jest 'esc'
        if(key == 27 ) capturing = false;
    } while( capturing );

    return 0;
}
