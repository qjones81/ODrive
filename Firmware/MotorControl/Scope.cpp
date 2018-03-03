#include "Scope.h"


// Global Scope for nOw
Scope scope;

Scope::Scope(float update_rate)
  : triggered_(false), started_(false), update_rate_(update_rate), sample_time_base_(1000) {
}

bool Scope::Start() {    // TODO: Run for time period?
    started_ = true;
    triggered_ = false;
    trigger_complete = false;
    is_triggering = false;

    // Reset Circular Buffers
    for (uint8_t i = 0; i < channels_.size(); i++) {
        channels_[i]->sample_buffer.clear();
    }

    // Sanity checks here.  Make sure memory is allocated, update rates are consistent and valid with sample rates, etc
    // Warn
    return started_;
}

void Scope::Stop() {
    started_ = false;

    //  Update Variables for python
    trigger_complete = true;
    is_triggering = false;
}
void Scope::Reset() {
    // Clear Channels
    for (uint8_t i = 0; i < channels_.size(); i++) {
        channels_[i]->sample_buffer.clear();
    }
    // Clear Triggers
    channels_.clear();
    Stop();
}

void Scope::SampleChannels() {
    if (!started_)
        return;

    static uint32_t triggered_sample_count = 0;
    static uint32_t triggered_buffer_size = 0;

    // We are setup here for basically a single shot mode. When done goes into Stop condition
    if (triggered_ && (triggered_sample_count++ >= triggered_buffer_size)) {
        Stop();
        return;
    }

    // If triggered sample stream from callback
    for (uint8_t i = 0; i < channels_.size(); i++) {
         channels_[i]->sample_buffer.push_back(*channels_[i]->config.signal_source);  // Sample at full rate.  TODO: decimate data if sample rate is lower
        // If not triggered, process channels for triggering
        if (!triggered_) {  // Evaluate Triggers
            // TODO: Give time to settle dwon before processing triggers
            triggered_ = ProcessChannelTriggerStatus(channels_[i]);
            if(triggered_) {
                is_triggering = true;
                triggered_sample_count = 0;
                triggered_buffer_size = sample_time_base_ * 1e-3 * update_rate_;
            }
        }
    }
}

 bool Scope::ProcessChannelTriggerStatus(uint8_t channel_id) {
     if(channel_id >= channels_.size()) // Make sure in bounds
        return false;

    return ProcessChannelTriggerStatus(channels_[channel_id]);
}
bool Scope::ProcessChannelTriggerStatus(Channel_t *channel) {

    // TODO: Also need to set the triggered time also for pre/post
    bool triggered = false;
    ChannelConfig_t config = channel->config;
    switch (config.trigger_type) {
        case TRIGGER_NONE:  // No Trigger Here
            triggered = false;
            break;
       // case TRIGGER_ALWAYS:  // Trigger on first read
         //   triggered = true;
           // break;
        case TRIGGER_EDGE:  // Edge Trigger
            triggered = ProcessEdgeTriggerStatus(channel);
            break;
        case TRIGGER_PULSE:  // Pulse Width Trigger
            break;
        case TRIGGER_SLOPE:  // Slope Trigger
            break;
        case TRIGGER_TIMEOUT:  // Timeout Trigger
            break;
        case TRIGGER_CUSTOM:  // Custom Defined Trigger
            triggered = config.signal_trigger_custom((void*)channel);
            break;
        default:
            triggered = false;
            break;
    }
    return triggered;
}
bool Scope::ProcessEdgeTriggerStatus(Channel_t *channel) {
    if (channel->sample_buffer.size() < 5) {  // TODO: More generic.  But wait for some n number of samples to come and settle things out
        return false;
    }

    float last_sample = channel->sample_buffer[channel->sample_buffer.size() - 2];
    float current_sample = channel->sample_buffer.back();

    bool triggered = false;
    ChannelConfig_t config = channel->config;
    switch (config.slope_type) {
        case EDGE_RISING:
            if ((last_sample < channel->config.trigger_level) && (current_sample >= channel->config.trigger_level))  // Low to High
                triggered = true;
            break;
        case EDGE_FALLING:
            if ((last_sample > channel->config.trigger_level) && (current_sample <= channel->config.trigger_level))  // High To Low
                triggered = true;
            break;
        case EDGE_RISING_FALLING:
            if (((last_sample < channel->config.trigger_level) && (current_sample >= channel->config.trigger_level))      // Low To High
                || ((last_sample > channel->config.trigger_level) && (current_sample <= channel->config.trigger_level)))  // High To Low
                triggered = true;
            break;
    }
    return triggered;
}
bool Scope::AddChannel(const ChannelConfig_t &config) {
    // Copy Configs
    Channel_t *channel = new Channel_t;
    channel->config = config;
    channel->last_sample_time = 0;
    // TODO: Make sure sample rate is < scope update rate.  Warn somehow
    // Setup Channel Buffers
    uint32_t buffer_size = sample_time_base_ * 1e-3 * config.sample_rate + 5; // A bit of padding for the hell of it
    if(!channel->sample_buffer.init(buffer_size))
        return false; // out of memory

    //if(!channel.memory_depth_buffer.init(config.memory_depth))
     //   return false;

    channels_.push_back(channel);
    return true;
}
void Scope::RemoveChannel(uint8_t channel_id) {
    if(channel_id >= channels_.size()) // Make sure in bounds
        return;
    channels_.erase(channels_.begin() + channel_id);
}
bool Scope::SetChannelSignalSource(uint8_t channel_id, float *signal_source) {
     if(channel_id >= channels_.size()) // Make sure in bounds
        return false;

    channels_[channel_id]->config.signal_source = signal_source;
    return true;
}
bool Scope::SetChannelSignalCustomTriggerCallback(uint8_t channel_id, trigger_callback callback) {
     if(channel_id >= channels_.size()) // Make sure in bounds
        return false;

    channels_[channel_id]->config.signal_trigger_custom = callback;
    return true;
}
CircularBuffer<float> *Scope::GetChannelSampleBuffer(uint8_t channel_id) {
    if(channel_id >= channels_.size()) // Make sure in bounds
        return nullptr;

    return &channels_[channel_id]->sample_buffer;
}
uint8_t inline Scope::ChannelCount() {
    return channels_.size();
}

// Python Read Out Function Helpers
void Scope::GetSampleBufferSize(uint8_t channel_id) {
    if (channel_id >= channels_.size())  // Make sure in bounds
        sample_buffer_size = 0;

    sample_buffer_size = channels_[channel_id]->sample_buffer.size();
}
void Scope::ReadSample(uint8_t channel_id, uint32_t sample_index) {
    if (channel_id >= channels_.size())  // Make sure in bounds
        sample_read_value = 0;

    sample_read_value = channels_[channel_id]->sample_buffer[sample_index];
}
