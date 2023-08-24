import math
import numpy as np
import matplotlib.pyplot as plt
import torch

p = 0.3275911
a1 = 0.254829592
a2 = -0.284496736
a3 = 1.421413741
a4 = -1.453152027
a5 = 1.061405429


def function_erf(x):
    x = torch.as_tensor(x)
    result = torch.erf(x)
    return result


def custom_erf(x):
    y = np.asarray(x)
    sign = np.sign(y)
    y = np.where(y < 0, -y, y)
    t = 1.0 / (1 + p * y)
    poly = a1 * t + a2 * t**2 + a3 * t**3 + a4 * t**4 + a5 * t**5
    result = 1.0 - poly * np.exp(-y * y)
    result *= sign
    return result


x = np.linspace(-4, 4, 50)
z = function_erf(x)
z1 = custom_erf(x)
plt.plot(x, z, "--g", label="erf")
plt.plot(x, z1, "+r", label="custom erf")
plt.legend(loc="upper center")
plt.show()
