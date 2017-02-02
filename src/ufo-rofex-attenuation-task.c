/*
 * Copyright (C) 2011-2015 Karlsruhe Institute of Technology
 *
 * This file is part of Ufo.
 *
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include "ufo-rofex-attenuation-task.h"


struct _UfoRofexAttenuationTaskPrivate {
    gboolean foo;
};

static void ufo_task_interface_init (UfoTaskIface *iface);

G_DEFINE_TYPE_WITH_CODE (UfoRofexAttenuationTask, ufo_rofex_attenuation_task, UFO_TYPE_TASK_NODE,
                         G_IMPLEMENT_INTERFACE (UFO_TYPE_TASK,
                                                ufo_task_interface_init))

#define UFO_ROFEX_ATTENUATION_TASK_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), UFO_TYPE_ROFEX_ATTENUATION_TASK, UfoRofexAttenuationTaskPrivate))

enum {
    PROP_0,
    PROP_TEST,
    N_PROPERTIES
};

static GParamSpec *properties[N_PROPERTIES] = { NULL, };

UfoNode *
ufo_rofex_attenuation_task_new (void)
{
    return UFO_NODE (g_object_new (UFO_TYPE_ROFEX_ATTENUATION_TASK, NULL));
}

static void
ufo_rofex_attenuation_task_setup (UfoTask *task,
                       UfoResources *resources,
                       GError **error)
{
}

static void
ufo_rofex_attenuation_task_get_requisition (UfoTask *task,
                                            UfoBuffer **inputs,
                                            UfoRequisition *requisition)
{
    UfoBuffer *data = inputs[0];
    ufo_buffer_get_requisition(data, requisition);
}

static guint
ufo_rofex_attenuation_task_get_num_inputs (UfoTask *task)
{
    return 2;
}

static guint
ufo_rofex_attenuation_task_get_num_dimensions (UfoTask *task,
                                             guint input)
{
    return 2;
}

static UfoTaskMode
ufo_rofex_attenuation_task_get_mode (UfoTask *task)
{
    return UFO_TASK_MODE_PROCESSOR | UFO_TASK_MODE_GPU;
}

static gboolean
ufo_rofex_attenuation_task_process (UfoTask *task,
                         UfoBuffer **inputs,
                         UfoBuffer *output,
                         UfoRequisition *requisition)
{
    UfoGpuNode *node;
    cl_command_queue cmd_queue;

    node = UFO_GPU_NODE (ufo_task_node_get_proc_node (UFO_TASK_NODE (task)));
    cmd_queue = ufo_gpu_node_get_cmd_queue (node);

    UfoBuffer *data = inputs[0];
    UfoBuffer *dark = inputs[1];

    gfloat *h_data = ufo_buffer_get_host_array(data, cmd_queue);
    gfloat *h_dark = ufo_buffer_get_host_array(dark, cmd_queue);
    gfloat *h_attenuation = ufo_buffer_get_host_array(output, cmd_queue);

    for (unsigned int i = 0; i < requisition->dims[0]; ++i) {
      for (unsigned int j = 0; j < requisition->dims[1]; ++j) {
        unsigned int index = i*requisition->dims[1] + j;
        h_attenuation[index] = h_data[index] + h_dark[index];
      }
    }

    return TRUE;
}


static void
ufo_rofex_attenuation_task_set_property (GObject *object,
                              guint property_id,
                              const GValue *value,
                              GParamSpec *pspec)
{
    UfoRofexAttenuationTaskPrivate *priv = UFO_ROFEX_ATTENUATION_TASK_GET_PRIVATE (object);

    switch (property_id) {
        case PROP_TEST:
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void
ufo_rofex_attenuation_task_get_property (GObject *object,
                              guint property_id,
                              GValue *value,
                              GParamSpec *pspec)
{
    UfoRofexAttenuationTaskPrivate *priv = UFO_ROFEX_ATTENUATION_TASK_GET_PRIVATE (object);

    switch (property_id) {
        case PROP_TEST:
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

static void
ufo_rofex_attenuation_task_finalize (GObject *object)
{
    G_OBJECT_CLASS (ufo_rofex_attenuation_task_parent_class)->finalize (object);
}

static void
ufo_task_interface_init (UfoTaskIface *iface)
{
    iface->setup = ufo_rofex_attenuation_task_setup;
    iface->get_num_inputs = ufo_rofex_attenuation_task_get_num_inputs;
    iface->get_num_dimensions = ufo_rofex_attenuation_task_get_num_dimensions;
    iface->get_mode = ufo_rofex_attenuation_task_get_mode;
    iface->get_requisition = ufo_rofex_attenuation_task_get_requisition;
    iface->process = ufo_rofex_attenuation_task_process;
}

static void
ufo_rofex_attenuation_task_class_init (UfoRofexAttenuationTaskClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS (klass);

    oclass->set_property = ufo_rofex_attenuation_task_set_property;
    oclass->get_property = ufo_rofex_attenuation_task_get_property;
    oclass->finalize = ufo_rofex_attenuation_task_finalize;

    properties[PROP_TEST] =
        g_param_spec_string ("test",
            "Test property nick",
            "Test property description blurb",
            "",
            G_PARAM_READWRITE);

    for (guint i = PROP_0 + 1; i < N_PROPERTIES; i++)
        g_object_class_install_property (oclass, i, properties[i]);

    g_type_class_add_private (oclass, sizeof(UfoRofexAttenuationTaskPrivate));
}

static void
ufo_rofex_attenuation_task_init(UfoRofexAttenuationTask *self)
{
    self->priv = UFO_ROFEX_ATTENUATION_TASK_GET_PRIVATE(self);
}