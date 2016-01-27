"""
A script validating the ImuFactor prediction and inference.
"""

import math
import matplotlib.pyplot as plt
import numpy as np

from mpl_toolkits.mplot3d import Axes3D

import gtsam
from gtsam_utils import plotPose3

class ImuFactorExample(object):

    @staticmethod
    def defaultParams(g):
        """Create default parameters with Z *up* and realistic noise parameters"""
        params = gtsam.PreintegrationParams.MakeSharedU(g)
        kGyroSigma = math.radians(0.5) / 60  # 0.5 degree ARW
        kAccelSigma = 0.1 / 60  # 10 cm VRW
        params.gyroscopeCovariance = kGyroSigma ** 2 * np.identity(3, np.float)
        params.accelerometerCovariance = kAccelSigma ** 2 * np.identity(3, np.float)
        params.integrationCovariance = 0.0000001 ** 2 * np.identity(3, np.float)
        return params

    def __init__(self):
        # setup interactive plotting
        plt.ion()

        # Setup loop scenario
        # Forward velocity 2m/s
        # Pitch up with angular velocity 6 degree/sec (negative in FLU)
        v = 2
        w = math.radians(30)
        W = np.array([0, -w, 0])
        V = np.array([v, 0, 0])
        self.scenario = gtsam.ConstantTwistScenario(W, V)
        self.dt = 1e-2

        # Calculate time to do 1 loop
        self.radius = v / w
        self.timeForOneLoop = 2.0 * math.pi / w
        self.labels = list('xyz')
        self.colors = list('rgb')

        # Create runner
        self.g = 10  # simple gravity constant
        self.params = self.defaultParams(self.g)
        ptr = gtsam.ScenarioPointer(self.scenario)
        accBias = np.array([0, 0.1, 0])
        gyroBias = np.array([0, 0, 0])
        self.actualBias = gtsam.ConstantBias(accBias, gyroBias)
        print(self.actualBias)
        self.runner = gtsam.ScenarioRunner(ptr, self.params, self.dt, self.actualBias)

    def plot(self, t, measuredOmega, measuredAcc):
        # plot angular velocity    
        omega_b = self.scenario.omega_b(t)
        plt.figure(1)
        for i, (label, color) in enumerate(zip(self.labels, self.colors)):
            plt.subplot(3, 1, i + 1)
            plt.scatter(t, omega_b[i], color='k', marker='.')
            plt.scatter(t, measuredOmega[i], color=color, marker='.')
            plt.xlabel(label)

        # plot acceleration in nav
        plt.figure(2)
        acceleration_n = self.scenario.acceleration_n(t)
        for i, (label, color) in enumerate(zip(self.labels, self.colors)):
            plt.subplot(3, 1, i + 1)
            plt.scatter(t, acceleration_n[i], color=color, marker='.')
            plt.xlabel(label)

        # plot acceleration in body
        plt.figure(3)
        acceleration_b = self.scenario.acceleration_b(t)
        for i, (label, color) in enumerate(zip(self.labels, self.colors)):
            plt.subplot(3, 1, i + 1)
            plt.scatter(t, acceleration_b[i], color=color, marker='.')
            plt.xlabel(label)

        # plot ground truth pose, as well as prediction from integrated IMU measurements
        actualPose = self.scenario.pose(t)
        plotPose3(4, actualPose, 0.3)
        pim = self.runner.integrate(t, self.actualBias, True)
        predictedNavState = self.runner.predict(pim, self.actualBias)
        plotPose3(4, predictedNavState.pose(), 0.1)
        ax = plt.gca()
        ax.set_xlim3d(-self.radius, self.radius)
        ax.set_ylim3d(-self.radius, self.radius)
        ax.set_zlim3d(0, self.radius * 2)

        # plot actual specific force, as well as corrupted
        plt.figure(5)
        actual = self.runner.actualSpecificForce(t)
        for i, (label, color) in enumerate(zip(self.labels, self.colors)):
            plt.subplot(3, 1, i + 1)
            plt.scatter(t, actual[i], color='k', marker='.')
            plt.scatter(t, measuredAcc[i], color=color, marker='.')
            plt.xlabel(label)

        plt.pause(0.01)

    def run(self):
        # simulate the loop up to the top
        T = self.timeForOneLoop
        for i, t in enumerate(np.arange(0, T, self.dt)):
            measuredOmega = self.runner.measuredAngularVelocity(t)
            measuredAcc = self.runner.measuredSpecificForce(t)
            if i % 25 == 0:
                self.plot(t, measuredOmega, measuredAcc)

        plt.ioff()
        plt.show()

if __name__ == '__main__':
    ImuFactorExample().run()
