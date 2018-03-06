#ifndef __SCOPE_H
#define __SCOPE_H

#include <vector>
#include <string>
#include "CircularBuffer.h"

struct Channel_t;
  typedef bool (*trigger_callback)(void *channel);

class Scope {
   public:
    typedef enum {
        TRIGGER_NONE,    // Triggering not enabled on this channel. Still logged and sampled
        //TRIGGER_ALWAYS, // TODO: Implement (Free Run Logger Mode to circular buffer?)  Probably not needed
        TRIGGER_EDGE,
        TRIGGER_PULSE,   //TODO: Implement
        TRIGGER_SLOPE,   //TODO: Implement
        TRIGGER_TIMEOUT,  //TODO: Implement
        TRIGGER_CUSTOM 
    } Trigger_type_t;

    typedef enum {
        EDGE_RISING,
        EDGE_FALLING,
        EDGE_RISING_FALLING
    } Trigger_slope_t;

    struct ChannelConfig_t {
        uint8_t sample_decimation; // collect data every 'nth' data point = to decimation.
        float trigger_level; 

        Trigger_type_t trigger_type;
        Trigger_slope_t slope_type;  // set for timeout or edge
        union {
            float trigger_pulse_width;
            float trigger_duration;
            float trigger_timeout;
        } settings;

        // Custom Trigger Callback
        trigger_callback signal_trigger_custom;

        // Also for functions?
        float *signal_source; // Source Variable
    };

    // TODO: Floats only for now.  Anything else can just be converted to this
    typedef struct  {
        ChannelConfig_t config;
        CircularBuffer<float> sample_buffer;     // Need some kind of max buffer size check.  50K for now?  50000 total samples
        uint8_t decimation_count;
        uint32_t trigger_offset;
    } Channel_t;

    // TODO: Total Maximum Samples per second?

    // Constructor             
    Scope(float update_rate = 10000);

    bool Start(); // TODO: Run for time period?
    void Stop(); // Stop Sampling
    void Reset(); // Reset Scope

    void SampleChannels(); // Callback to read in and sample channel data

    // Function or Variable
    bool SetChannelSignalSource(uint8_t channel_id, float *signal_source);
    bool SetChannelSignalCustomTriggerCallback(uint8_t channel_id, trigger_callback callback);
    bool SetTriggeredMeasurementsReadyCallback(trigger_callback callback);
    bool AddChannel(const ChannelConfig_t &config);
    void RemoveChannel(uint8_t channel_id); // Zero Indexed
    uint8_t inline ChannelCount(); // Get Number of active channels
    CircularBuffer<float> *GetChannelSampleBuffer(uint8_t channel_id);

    // Set/Gets
    void inline set_update_rate(float update_rate) { update_rate_ = update_rate; } // TODO: Reconfigure channel buffer sizes
    float inline get_update_rate() { return update_rate_; }
    void inline set_sample_time_base(float sample_time_base) { sample_time_base_ = sample_time_base; } // TODO: Reconfigure channel buffer sizes
    float inline get_sample_time_base() { return sample_time_base_; }
    void inline set_pretrigger_time_base(float pretrigger_time_base) { pretrigger_time_base_ = pretrigger_time_base; } // TODO: Reconfigure channel buffer sizes
    float inline get_pretrigger_time_base() { return pretrigger_time_base_; }

    bool get_triggered() { return triggered_;}
    // Add Triggered Callback

    // Python Command Interface Mappings
    struct {
        uint8_t channel_id;
    } set_channel_read_source_args;

    struct {
        uint8_t channel_id;
        uint32_t index;
    } set_channel_read_sample_args;

    void GetSampleBufferSize(uint8_t channel_id);
    void GetChannelSampleDecimation(uint8_t channel_id);
    void GetChannelTriggerOffset(uint8_t channel_id);
    void ReadSample(uint8_t channel_id, uint32_t sample_index);
    float sample_read_value;
    uint32_t sample_buffer_size;
    uint32_t sample_decimation;
    uint32_t trigger_offset;
    uint32_t trigger_complete;
    uint32_t is_triggering;


    // TODO: Add "User Signals Map Here:" std::map<string, float *>
    // "AddSignalMeasurement("Name", float *ptr);"
    // "AddChannel(Blah Blah)
    // "ConnectSignal(Channel, Signal Name)""

    // End Python.  Can this go somewhere else?  Maybe helper libs of some kind. or a "Reader Interface"
    // For now only sample "Triggered" channels then start processing all

   private: 

    // Look for any setup triggers that have setup
    bool ProcessChannelTriggerStatus(Channel_t *channel);
    bool ProcessChannelTriggerStatus(uint8_t channel_id);

    bool ProcessEdgeTriggerStatus(Channel_t *channel);
    bool triggered_; // Scope has been triggered by a setup channel input
    bool started_;  // Scope in Run mode
    float update_rate_; // Rate for Sample data update rate.  Channel sampling rates are decimated and downsampled from this    
    // TODO: Decimation on main scope?  I think this should be timer adjusted.

    float sample_time_base_;  // ms (How long are sampling for)
    float pretrigger_time_base_; // ms (How much buffer time)  Should these really be in "num samples"?

    // Sampled Channels
    std::vector<Channel_t *> channels_;
};

extern Scope scope;
#endif /* __SCOPE_H */
