#include <iostream>

#include "File.hpp"
#include "Storage.hpp"
#include <simgrid/s4u/Engine.hpp>
#include <simgrid/Exception.hpp>

namespace simgrid::module::fs {

    /**
     * @brief Write data to the file
     * @param num_bytes: the number of bytes to write
     */
    void File::write(const std::string& num_bytes, bool simulate_it) {
        write(static_cast<sg_size_t>(xbt_parse_get_size("", 0, num_bytes, "")), simulate_it);
    }

    void File::write(sg_size_t num_bytes, bool simulate_it) {
        static int sequence_number = -1;
        int my_sequence_number = ++sequence_number;

        //TODO: Would be good to move some of the code below to FileMetadata, but that requires
        //      that FileMetadata know the partition....

        // Check whether there is enough space
        sg_size_t added_bytes = std::max<sg_size_t >(0, current_position_ + num_bytes - metadata_->get_future_size());
        if (added_bytes > partition_->get_free_space()) {
            throw std::runtime_error("EXCEPTION"); // TODO
        }

        // Compute the new tentative file size
        sg_size_t new_file_size_if_i_succeed = metadata_->get_future_size() + added_bytes;

        metadata_->notify_write_start(my_sequence_number, new_file_size_if_i_succeed);

        // "Reserve" space for the write operation
        partition_->decrease_free_space(new_file_size_if_i_succeed - metadata_->get_current_size());


        // Do the I/O simulation if need be
        if (simulate_it) {
            try {
                partition_->get_storage()->write(num_bytes);
            } catch (StorageFailureException &e) {
                throw xbt::UnimplementedError("Handling of hardware resource failures not implemented");
            }
        }


        // Update
        metadata_->set_access_date(s4u::Engine::get_clock());
        metadata_->set_modification_date(s4u::Engine::get_clock());
        metadata_->notify_write_end(my_sequence_number);
    }

    void File::update_current_position(sg_offset_t pos) {
        xbt_assert(pos >= 0, "Error in seek, cannot seek before file %s", path_.c_str());
        current_position_ = pos;
        metadata_->set_access_date(s4u::Engine::get_clock());

        if(current_position_ > metadata_->get_current_size()) {
            // XBT_DEBUG("Updating size of file %s from %llu to %lld", path_.c_str(),
            //           metadata_->get_current_size(), pos);
            partition_->decrease_free_space(pos - metadata_->get_current_size());
            metadata_->set_current_size(current_position_);
            metadata_->set_modification_date(s4u::Engine::get_clock());
        }
    }

    /**
     * @brief Seek to a position in the file
     * @param pos: the position as an offset from the first byte of the file
     */
    void File::seek(sg_offset_t pos) {
        update_current_position(pos);
    }

    /**
     * @brief Seek to a position in the file from a given origin
     * @param pos: the position as an offset from the given origin in the file
     * @param origin: where to start adding the offset in the file
     */
    void File::seek(sg_offset_t pos, int origin) {
        switch (origin) {
            case SEEK_SET:
                update_current_position(pos);
                break;
            case SEEK_CUR:
                update_current_position(current_position_ + pos);
                break;
            case SEEK_END:
                update_current_position(metadata_->get_current_size() + pos);
                break;
            default:
                update_current_position(origin + pos);
                break;
        }
    }

    /**
     * @brief Closes the file. After closing, using the file has undefined
     * behavior.
     */
    void File::close() {
        // nothing?
    }

}