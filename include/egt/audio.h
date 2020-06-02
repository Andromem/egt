/*
 * Copyright (C) 2018 Microchip Technology Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EGT_AUDIO_H
#define EGT_AUDIO_H

/**
 * @file
 * @brief Working with audio.
 */

/**
 * @defgroup media Images and Video
 * Image and video related classes.
 */

#include <egt/detail/meta.h>
#include <egt/object.h>
#include <egt/signal.h>
#include <memory>
#include <string>

namespace egt
{
inline namespace v1
{

namespace detail
{
struct AudioPlayerImpl;
}

/**
 * Audio player.
 *
 * @ingroup media
 */
class EGT_API AudioPlayer : public Object
{
public:

    /**
     * Event signal.
     * @{
     */
    /**
     * Invoked when the position of the player changes.
     */
    Signal<uint64_t> on_position_changed;

    /**
     * Invoked when an error occurs.
     */
    Signal<const std::string&> on_error;

    /**
     * Invoked on end of stream.
     */
    Signal<> on_eos;

    /**
     * Invoked when the state of the player changes.
     */
    Signal<> on_state_changed;
    /** @} */

    AudioPlayer();

    /**
     * Construct and set the media file URI to the current pipeline.
     * @param uri file URI
     */
    explicit AudioPlayer(const std::string& uri);

    EGT_OPS_NOCOPY_MOVE(AudioPlayer);
    ~AudioPlayer() noexcept override;

    /**
     * Sets the media file URI to the current pipeline
     * @param uri file URI
     * @return true on success
     */
    bool media(const std::string& uri);

    /**
     * Send pipeline to play state
     * @return true on success
     */
    bool play();

    /**
     * pause Send Pipeline to pause state
     * @return true on success
     */
    bool pause();

    /**
     * Adjusts the volume of the audio being played.
     *
     * @param volume Value in the range 0 - 100.
     * @return true on success
     */
    bool volume(int volume);

    /**
     * Get the volume.
     * @return Value in the range 0 - 100.
     */
    EGT_NODISCARD int volume() const;

    /**
     * Mutes the audio being played.
     *
     * @param mute When true, audio is to be muted.
     * @return true on success
     */
    bool mute(bool mute);

    /**
     * Get the current position of the audio stream.
     *
     * @return Time duration in nanoseconds.
     */
    EGT_NODISCARD uint64_t position() const;

    /**
     * Get the duration of the audio stream.
     *
     * @return Time duration in nanoseconds.
     */
    EGT_NODISCARD uint64_t duration() const;

    /**
     * Seek to a position.
     *
     * The position is given by the position() function and the duration is
     * given by the duration() method.
     *
     * @param pos Position in nanoseconds.
     * @return true on success
     */
    bool seek(uint64_t pos);

    /**
     * Returns true if the stream is currently playing.
     */
    EGT_NODISCARD bool playing() const;

protected:

    /**
     * Send pipeline to null state
     * @return true on success
     */
    bool null();

    /// Create the pipeline.
    bool createPipeline(const std::string& uri);

    /// Destroy the pipeline.
    void destroyPipeline();

    /// Implementation pointer.
    std::unique_ptr<detail::AudioPlayerImpl> m_impl;
};

}
}

#endif
