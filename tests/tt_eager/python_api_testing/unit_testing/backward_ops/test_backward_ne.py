# SPDX-FileCopyrightText: © 2023 Tenstorrent Inc.

# SPDX-License-Identifier: Apache-2.0

import torch
import pytest
import tt_lib
from tests.tt_eager.python_api_testing.unit_testing.backward_ops.utility_funcs import data_gen_with_range, compare_pcc


@pytest.mark.parametrize(
    "input_shapes",
    (
        (torch.Size([1, 1, 32, 32])),
        (torch.Size([1, 1, 320, 384])),
        (torch.Size([1, 3, 320, 384])),
    ),
)
def test_bw_unary_ne(input_shapes, device):
    grad_data, grad_tensor = data_gen_with_range(input_shapes, -100, 100, device)
    tt_output_tensor_on_device = tt_lib.tensor.ne_bw(grad_tensor)
    pyt_y = torch.zeros_like(grad_data)

    golden_tensor = [pyt_y]

    comp_pass = compare_pcc(tt_output_tensor_on_device, golden_tensor)
    assert comp_pass
