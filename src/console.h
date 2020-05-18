/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
 * Copyright 2019  Paul Derbyshire
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EMSESP_CONSOLE_H
#define EMSESP_CONSOLE_H

#include <Arduino.h>

#include <string>
#include <vector>

#include <uuid/console.h>
#include <uuid/log.h>

#include "helpers.h"
#include "settings.h"
#include "system.h"
#include "network.h"
#include "mqtt.h"

using uuid::flash_string_vector;
using uuid::read_flash_string;
using uuid::console::Commands;
using uuid::console::Shell;
using uuid::log::Level;

// clang-format off

#define DEBUG_LOG(...) if (logger_.enabled(Level::DEBUG)) {logger_.debug(__VA_ARGS__);}
#define TRACE_LOG(...) if (logger_.enabled(Level::TRACE)) {logger_.trace(__VA_ARGS__);}

#define MAKE_PSTR(string_name, string_literal) static const char __pstr__##string_name[] __attribute__((__aligned__(sizeof(int)))) PROGMEM = string_literal;
#define MAKE_PSTR_WORD(string_name) MAKE_PSTR(string_name, #string_name)
#define F_(string_name) FPSTR(__pstr__##string_name)
#define NO_ARGUMENTS std::vector<std::string>{}

// clang-format on

// common words
#ifdef EMSESP_DEBUG
MAKE_PSTR_WORD(test)
#endif
MAKE_PSTR_WORD(exit)
MAKE_PSTR_WORD(help)
MAKE_PSTR_WORD(settings)
MAKE_PSTR_WORD(log)
MAKE_PSTR_WORD(logout)
MAKE_PSTR_WORD(enabled)
MAKE_PSTR_WORD(disabled)
MAKE_PSTR_WORD(yes)
MAKE_PSTR_WORD(no)
MAKE_PSTR_WORD(set)
MAKE_PSTR_WORD(show)
MAKE_PSTR_WORD(on)
MAKE_PSTR_WORD(off)
MAKE_PSTR_WORD(su)
MAKE_PSTR_WORD(name)
MAKE_PSTR_WORD(auto)
MAKE_PSTR_WORD(scan)
MAKE_PSTR_WORD(password)
MAKE_PSTR_WORD(read)
MAKE_PSTR_WORD(version)
MAKE_PSTR_WORD(values)
MAKE_PSTR_WORD(system)
MAKE_PSTR_WORD(refresh)
MAKE_PSTR_WORD(change)
MAKE_PSTR_WORD(disconnect)
MAKE_PSTR_WORD(debug)
MAKE_PSTR_WORD(restart)
MAKE_PSTR_WORD(reconnect)
MAKE_PSTR_WORD(format)

// context menus
MAKE_PSTR_WORD(mqtt)
MAKE_PSTR_WORD(ems)

MAKE_PSTR(percent, "%")
MAKE_PSTR(degrees, "°C")
MAKE_PSTR(degrees_mandatory, "<degrees>")
MAKE_PSTR(asterisks, "********")
MAKE_PSTR(n_mandatory, "<n>")
MAKE_PSTR(n_optional, "[n]")
MAKE_PSTR(traceid_optional, "[trace ID]")
MAKE_PSTR(bool_mandatory, "<on | off>")
MAKE_PSTR(typeid_mandatory, "<type ID>")
MAKE_PSTR(deviceid_mandatory, "<device ID>")
MAKE_PSTR(deviceid_optional, "[device ID]")
MAKE_PSTR(invalid_log_level, "Invalid log level")
MAKE_PSTR(ip_address_optional, "[IP address]")
MAKE_PSTR(ip_address_mandatory, "<IP address>")
MAKE_PSTR(log_level_fmt, "Log level = %s")
MAKE_PSTR(log_level_optional, "[level]")
MAKE_PSTR(name_mandatory, "<name>")
MAKE_PSTR(name_optional, "[name]")
MAKE_PSTR(new_password_prompt1, "Enter new password: ")
MAKE_PSTR(new_password_prompt2, "Retype new password: ")
MAKE_PSTR(password_prompt, "Password: ")
MAKE_PSTR(seconds_optional, "[seconds]")
MAKE_PSTR(seconds_mandatory, "<seconds>")
MAKE_PSTR(unset, "<unset>")

#ifdef LOCAL
#undef LOCAL
#endif

static constexpr uint32_t INVALID_PASSWORD_DELAY_MS = 3000;

namespace emsesp {

using LogLevel    = ::uuid::log::Level;
using LogFacility = ::uuid::log::Facility;

enum CommandFlags : uint8_t { USER = 0, ADMIN = (1 << 0), LOCAL = (1 << 1) };

enum ShellContext : uint8_t {

    MAIN = 0,
    SYSTEM,
    EMS,
    MQTT,
    BOILER,
    THERMOSTAT

};

class EMSESPShell : virtual public uuid::console::Shell {
  public:
    ~EMSESPShell() override = default;

    virtual std::string console_name() = 0;

    static std::shared_ptr<uuid::console::Commands> commands;
    static std::shared_ptr<EMSESPShell>             shell;

    void enter_custom_context(unsigned int context);

  protected:
    EMSESPShell();

    // our custom functions for Shell
    void        started() override;
    void        stopped() override;
    void        display_banner() override;
    std::string hostname_text() override;
    std::string context_text() override;
    std::string prompt_suffix() override;
    void        end_of_transmission() override;
    bool        exit_context() override;

  private:
    void add_console_commands();
    bool _console_commands_loaded = false; // set to true when the initial commands are loaded
};

class EMSESPStreamConsole : public uuid::console::StreamConsole, public EMSESPShell {
  public:
    EMSESPStreamConsole(Stream & stream, bool local);
    EMSESPStreamConsole(Stream & stream, const IPAddress & addr, uint16_t port);
    ~EMSESPStreamConsole() override;

    std::string console_name() override;

  private:
    static std::vector<bool> ptys_;

    std::string name_;
    size_t      pty_;
    IPAddress   addr_;
    uint16_t    port_;
};

class Console {
  public:
    void loop();
    void start();

    uuid::log::Level log_level();

  private:
    static constexpr unsigned long SERIAL_CONSOLE_BAUD_RATE = 115200;
    static constexpr auto &        serial_console_          = Serial;
};

} // namespace emsesp

#endif