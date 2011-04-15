#include <glib.h>
#include <gmodule.h>

#include "ufo-filter.h"

G_DEFINE_TYPE(UfoFilter, ufo_filter, ETHOS_TYPE_PLUGIN);

#define UFO_FILTER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), UFO_TYPE_FILTER, UfoFilterPrivate))

enum {
    FINISHED,
    LAST_SIGNAL
};

enum {
    PROP_0,
    PROP_NAME
};

struct _UfoFilterPrivate {
    UfoResourceManager  *resource_manager;
    GAsyncQueue         *input_queue;
    GAsyncQueue         *output_queue;
    gchar               *name;
};

static guint filter_signals[LAST_SIGNAL] = { 0 };

/* 
 * Public Interface
 */
void ufo_filter_set_resource_manager(UfoFilter *self, UfoResourceManager *resource_manager)
{
    self->priv->resource_manager = resource_manager;
}

UfoResourceManager *ufo_filter_get_resource_manager(UfoFilter *self)
{
    return self->priv->resource_manager;
}

void ufo_filter_set_input_queue(UfoFilter *self, GAsyncQueue *queue)
{
    if (queue != NULL)
        g_async_queue_ref(queue);
    self->priv->input_queue = queue;
}

void ufo_filter_set_output_queue(UfoFilter *self, GAsyncQueue *queue)
{
    if (queue != NULL)
        g_async_queue_ref(queue);
    self->priv->output_queue = queue;
}

GAsyncQueue *ufo_filter_get_input_queue(UfoFilter *self)
{
    return self->priv->input_queue;
}

GAsyncQueue *ufo_filter_get_output_queue(UfoFilter *self)
{
    return self->priv->output_queue;
}

/* 
 * Virtual Methods
 */
static void ufo_filter_set_property(GObject *object,
    guint           property_id,
    const GValue    *value,
    GParamSpec      *pspec)
{
    UfoFilter *self = UFO_FILTER(object);

    switch (property_id) {
        case PROP_NAME:
            g_free(self->priv->name);
            self->priv->name = g_value_dup_string(value);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

static void ufo_filter_get_property(GObject *object,
    guint       property_id,
    GValue      *value,
    GParamSpec  *pspec)
{
    UfoFilter *self = UFO_FILTER(object);

    switch (property_id) {
        case PROP_NAME:
            g_value_set_string(value, self->priv->name);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

void ufo_filter_process(UfoFilter *self)
{
    g_return_if_fail(UFO_IS_FILTER(self));
    UFO_FILTER_GET_CLASS(self)->process(self);
}

static void ufo_filter_process_default(UfoFilter *self)
{
    g_return_if_fail(UFO_IS_FILTER(self));
}

static void ufo_filter_dispose(GObject *object)
{
    UfoFilter *self = UFO_FILTER(object);
    g_async_queue_unref(self->priv->input_queue);
    g_async_queue_unref(self->priv->output_queue);
    G_OBJECT_CLASS(ufo_filter_parent_class)->dispose(object);
}


/*
 * Type/Class Initialization
 */
static void ufo_filter_class_init(UfoFilterClass *klass)
{
    /* override GObject methods */
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->set_property = ufo_filter_set_property;
    gobject_class->get_property = ufo_filter_get_property;
    gobject_class->dispose = ufo_filter_dispose;
    klass->process = ufo_filter_process_default;

    /* install properties */
    GParamSpec *pspec;

    pspec = g_param_spec_string("filter-name",
        "Name of the filter",
        "Get filter name",
        "no-name-set",
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READABLE);
    g_object_class_install_property(gobject_class, PROP_NAME, pspec);

    /* install signals */
    filter_signals[FINISHED] =
        g_signal_newv("finished",
                G_TYPE_FROM_CLASS(klass),
                G_SIGNAL_RUN_FIRST,
                NULL, /* no class closure, elements are connecting to us */
                NULL, NULL,
                g_cclosure_marshal_VOID__VOID,
                G_TYPE_NONE, 0, NULL);

    /* install private data */
    g_type_class_add_private(klass, sizeof(UfoFilterPrivate));
}

static void ufo_filter_init(UfoFilter *self)
{
    UfoFilterPrivate *priv;
    self->priv = priv = UFO_FILTER_GET_PRIVATE(self);
    priv->input_queue = NULL;
    priv->output_queue = NULL;
}

