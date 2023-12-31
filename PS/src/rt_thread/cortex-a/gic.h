/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-07-20     Bernard      first version
 */

#ifndef __GIC_H__
#define __GIC_H__

#include <rthw.h>
#include <board.h>

#define __REG32(x) (*((volatile unsigned int*)((rt_uint32_t)x)))

int arm_gic_get_active_irq(rt_uint32_t index);
void arm_gic_ack(rt_uint32_t index, int irq);

void arm_gic_mask(rt_uint32_t index, int irq);
void arm_gic_umask(rt_uint32_t index, int irq);

rt_uint32_t arm_gic_get_pending_irq(rt_uint32_t index, int irq);
void arm_gic_set_pending_irq(rt_uint32_t index, int irq);
void arm_gic_clear_pending_irq(rt_uint32_t index, int irq);

void arm_gic_set_configuration(rt_uint32_t index, int irq, rt_uint32_t config);
rt_uint32_t arm_gic_get_configuration(rt_uint32_t index, int irq);

void arm_gic_clear_active(rt_uint32_t index, int irq);

void arm_gic_set_cpu(rt_uint32_t index, int irq, unsigned int cpumask);
rt_uint32_t arm_gic_get_target_cpu(rt_uint32_t index, int irq);

void arm_gic_set_priority(rt_uint32_t index, int irq, rt_uint32_t priority);
rt_uint32_t arm_gic_get_priority(rt_uint32_t index, int irq);

void arm_gic_set_interface_prior_mask(rt_uint32_t index, rt_uint32_t priority);
rt_uint32_t arm_gic_get_interface_prior_mask(rt_uint32_t index);

void arm_gic_set_binary_point(rt_uint32_t index, rt_uint32_t binary_point);
rt_uint32_t arm_gic_get_binary_point(rt_uint32_t index);

rt_uint32_t arm_gic_get_irq_status(rt_uint32_t index, int irq);

void arm_gic_send_sgi(rt_uint32_t index, int irq, rt_uint32_t target_list, rt_uint32_t filter_list);

rt_uint32_t arm_gic_get_high_pending_irq(rt_uint32_t index);

rt_uint32_t arm_gic_get_interface_id(rt_uint32_t index);

void arm_gic_set_group(rt_uint32_t index, int irq, rt_uint32_t group);
rt_uint32_t arm_gic_get_group(rt_uint32_t index, int irq);

int arm_gic_dist_init(rt_uint32_t index, rt_uint32_t dist_base, int irq_start);
int arm_gic_cpu_init(rt_uint32_t index, rt_uint32_t cpu_base);

void arm_gic_dump_type(rt_uint32_t index);
void arm_gic_dump(rt_uint32_t index);

#endif

