## CarND-PID-Controls
This project implements a PID controller for keeping the car on track by adjusting the steering angle and throttle appropiately.

[//]: # (Image Reference)

[image1]: ./images/01_p_controller.png "P controller"
[image2]: ./images/02_i_controller.png "I controller"
[image3]: ./images/03_d_controller.png "D controller"

### Overview
A **proportional-integral-derivative controller** (PID) constantly calculates an **error function** (which in this case, is the distance from the centre of the lane) and applies a correction. I will demonstrate the effect of each controller.

#### 1. Effect of proportional controller:
**Proportional gain** regulates how large the change to steering angle will be for a given change in error.
![alt text][image1]
As is shown from the picture, the controller adjusts the steering but it always overshoots, resulting an oscillation.

#### 2. Effect of an added derivative controller:
**Derivative gain** describes how much the error's rate of change is taken into account when computing the response.
![alt text][image3]
From the picture, a PD controller can adjusts the steering angle and solve the overshooting problem by reducing the steering angle when error decreases. However, this controller cannot solve a constant error (system bias, steering drift)

#### 3. Effect of an added integrative controller:
**Integral gain** contributes in proportion to both the magnitude of the error and the duration of the error.
![alt text][image2]
The integrative controller can adjust the steering angle by calculate an integral of all errors. Therefore it captures constant error.

### Weakness of PID controller
A PID controller adjuests 'future' action with regard to 'current' measure. When the action takes effect the measure information is already out-dated. Hence a PID controller will act as if it has a delay. This determines that a PID controller will not work well under high speed because the 'delay' will severely affect the system.

### Decision process design and Parameter tuning
In this project I have listed this situations and designed a specific throttle and steering angle setting for each of them.
1. **start up or low speed** - quickly speed up to normal speed
2. **cruise in staight line with maximum speed** - use throttle intermittently to maintain maximum speed, the steering angle has been set to have an absolute maximum to keep the system stable
3. **driving around a bend** - a bend is shown by a large deviation from the central lane position, firstly the car decreases speed then use low speed and large steering angle combination to steer out of the bend

In this project, the parameter is tuned manually.

### Dependencies

* cmake >= 3.5
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `./install-mac.sh` or `./install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.
    ```
    git clone https://github.com/uWebSockets/uWebSockets 
    cd uWebSockets
    git checkout e94b6e1
    ```
    Some function signatures have changed in v0.14.x. See [this PR](https://github.com/udacity/CarND-MPC-Project/pull/3) for more details.
* Simulator. You can download these from the [project intro page](https://github.com/udacity/self-driving-car-sim/releases) in the classroom.

### Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./pid`. 