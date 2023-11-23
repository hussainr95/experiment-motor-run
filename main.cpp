#include <string>
#include <iostream>
#include <fstream>

/// HOW-TO-RUN
// Compile the code and run it as no specific flags need to be set in cmake command due to no dependencies.
// The text file "experiment_log.txt" will be generated and placed in the build folder which shall contain all the results.
// Values can be tweaked in the main function for desired results.

class Motor {
public:
    void setSpeed(double percent) {
        // Assumed to be implemented already
    }
};

class Fct {
public:
    Fct(std::string &name) {
        mName = name;
    }

    virtual void start(int timeMs, double amplitudePercent) = 0;

    virtual bool update() = 0;

    std::string getName() {
        return mName;
    }

private:
    std::string mName;
};

class MotorController : public Fct {
public:
    MotorController(std::string &name, Motor *motor)
            : Fct(name) {
        mMotor = motor;
    }

    ~MotorController() {
        delete mMotor;
    }

    void start(int timeMs, double amplitudePercent) {
        mtimeMs = timeMs;
        mamplitudePercent = amplitudePercent;
        mrampLimit = (20.0 / 100.0) * mtimeMs;
        mspeed = mamplitudePercent / mrampLimit;

        std::ofstream experimentLog("experiment_log.txt", std::ios::app);

        experimentLog << getName() << " details\n";
        experimentLog << "mtimeMs: " << mtimeMs << "\n";
        experimentLog << "mamplitudePercent: " << mamplitudePercent << "\n";
        experimentLog << "mrampLimit: " << mrampLimit << "\n";
        experimentLog << "mspeed: " << mspeed << "\n\n";

        experimentLog.close();
    }

    bool update() {
        std::ofstream experimentLog("experiment_log.txt", std::ios::app);

        std::string state;

        if (mrampCount < mrampLimit) {
            state = "Ramp up";
            experimentLog << getName() << "  ->\t\tmspeedChange: " << mspeedChange << "\t\tmrampCount: "
                          << mrampCount << "\t\tstate: " << state << "\n";
            mspeedChange += mspeed;
            mMotor->setSpeed(mspeedChange);
            mrampCount++;
        } else if (mrampCount >= mrampLimit && mrampCount < (mtimeMs - mrampLimit)) {
            state = "Steady speed";
            experimentLog << getName() << "  ->\t\tmspeedChange: " << mspeedChange << "\t\tmrampCount: "
                          << mrampCount << "\t\tstate: " << state << "\n";
            mMotor->setSpeed(mspeedChange);
            mrampCount++;
        } else if (mrampCount >= (mtimeMs - mrampLimit)) {
            state = "Ramp down";
            if (mrampCount > mtimeMs) {
                state = "Complete";
                experimentLog << getName() << "  ->\t\tmspeedChange: " << std::max(0.0, mspeedChange)
                              << "\t\tmrampCount: "
                              << mrampCount << "\t\tstate: " << state << "\n";
            } else {
                experimentLog << getName() << "  ->\t\tmspeedChange: " << std::max(0.0, mspeedChange)
                              << "\t\tmrampCount: "
                              << mrampCount << "\t\tstate: " << state << "\n";
            }
            mspeedChange -= mspeed;
            mMotor->setSpeed(std::max(0.0, mspeedChange));
            mrampCount++;
        }

        experimentLog.close();

        if (mrampCount > mtimeMs) {
            return true;
        } else {
            return false;
        }
    }

protected:
    Motor *mMotor;
    int mtimeMs;
    double mamplitudePercent, mrampLimit, mrampCount, mspeed;
    double mspeedChange = 0;
};

int main() {
    std::ofstream experimentLogStart("experiment_log.txt", std::ios::app);
    experimentLogStart << "******************* NEW EXPERIMENT *******************\n\n";
    experimentLogStart.close();

    std::string left_motor_controller = "Left motor controller";
    std::string right_motor_controller = "Right motor controller";

    Fct *controllerLeft = new MotorController(left_motor_controller, new Motor());
    Fct *controllerRight = new MotorController(right_motor_controller, new Motor());

    controllerLeft->start(10000, 75);
    controllerRight->start(5000, 50);

    bool leftControllerState = false;
    bool rightControllerState = false;

    while (!leftControllerState || !rightControllerState) {
        leftControllerState = controllerLeft->update();
        rightControllerState = controllerRight->update();
    }
    std::ofstream experimentLogFinish("experiment_log.txt", std::ios::app);
    experimentLogFinish << "******************* END EXPERIMENT *******************\n\n";
    experimentLogFinish.close();

    return 0;
}
