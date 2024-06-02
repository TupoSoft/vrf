//
// Created by Tuposoft Collective on 23.01.2023.
//

#include "EmailVerification.hpp"

#include <windns.h>

#include <format>
#include <memory>
#include <vector>

using namespace TupoSoft::VRF;

// static VRF_err_t
// send_command(int sock, char *format, ...) {
//     va_list args;
//     va_start(args, format);
//     char *command;
//     if (vasprintf(&command, format, args) < 0) {
//         return VRF_ERR;
//     }
//
//     if (verbose) printf("REQUEST: %s", command);
//
//     if (send(sock, command, strlen(command), 0) < 0) {
//         return VRF_ERR;
//     }
//     va_end(args);
//     free(command);
//
//     return VRF_OK;
// }

// static VRF_err_t
// read_response(int sock, char *buffer) {
//     auto b = (char (*)[SMTP_DATA_LINES_MAX_LENGTH]) buffer;
//     ssize_t nbytes;
//     if ((nbytes = read(sock, *b, sizeof *b)) < 0) {
//         printf("Failed to read from socket.\n");
//         return VRF_ERR;
//     }
//
//     if (verbose) printf("RESPONSE: %s", (char *) b);
//
//     //    memset(*b, 0, nbytes);
//     return VRF_OK;
// }

auto TupoSoft::VRF::extractLocalPartAndDomain(const std::string &email) -> std::pair<std::string, std::string> {
    if (const auto atPosition = email.find('@'); atPosition != std::string::npos) {
        auto username = email.substr(0, atPosition);
        auto domain = email.substr(atPosition + 1);
        return {username, domain};
    }

    throw std::invalid_argument("Invalid email format");
}

// static VRF_err_t
// check_mx(char *email, struct addrinfo *adrrinfo, EmailVerificationData *result) {
//     int sock, client_fd;
//     if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
//         printf("Failed to create a socket.\n");
//         return VRF_ERR;
//     }
//
//     char buffer[SMTP_DATA_LINES_MAX_LENGTH];
//     if ((client_fd = connect(sock, (struct sockaddr *) adrrinfo->ai_addr, sizeof(struct sockaddr))) < 0) {
//         printf("Connection failed.\n");
//         return VRF_ERR;
//     }
//
//     if (verbose) printf("SUCCESSFULLY CONNECTED TO %s\n", (*result)->mx_record);
//
//     int err;
//     CHECK_OK(read_response(sock, buffer), err)
//     CHECK_OK(send_command(sock, "EHLO %s\n", CLIENT_MX), err)
//     CHECK_OK(read_response(sock, buffer), err)
//     CHECK_OK(send_command(sock, "MAIL FROM: <%s>\n", CLIENT_EMAIL), err)
//     CHECK_OK(read_response(sock, buffer), err)
//     CHECK_OK(send_command(sock, "RCPT TO: <%s>\n", email), err)
//     CHECK_OK(read_response(sock, buffer), err)
//     char status[4];
//     memcpy(status, buffer, 3);
//     status[3] = '\0';
//     long code = strtol(status, NULL, 0);
//     if (!code) return VRF_ERR;
//     (*result)->result = code == 250;
//
//     CHECK_OK(send_command(sock, "QUIT\n"), err);
//
//     return !close(client_fd) ? VRF_OK : VRF_ERR;
// }

auto printVerificationData(std::ostream &os, EmailVerificationData emailVerificationData) -> std::ostream & {
    os << std::format("\nVerification summary:\n"
                      "email: {}\n"
                      "local part: {}\n"
                      "domain: {}\n"
                      "mx record: {}\n"
                      "result: {}\n"
                      "catch_all: {}\n\n",
                      emailVerificationData.email,
                      emailVerificationData.username,
                      emailVerificationData.domain,
                      emailVerificationData.mxRecord,
                      emailVerificationData.result == EmailVerificationResult::Success ? "true" : "false",
                      emailVerificationData.catchAll ? "true" : "false");

    return os;
}

auto TupoSoft::VRF::getMXRecords(const std::string &domain) -> std::vector<std::string> {
    PDNS_RECORD pDnsRecord{};

    if (DnsQuery_A(domain.c_str(), DNS_TYPE_MX, DNS_QUERY_STANDARD, nullptr, &pDnsRecord, nullptr)) {
        throw std::runtime_error("DNS query failed with error code: " + std::to_string(
                                     DnsQuery_A(domain.c_str(), DNS_TYPE_MX, DNS_QUERY_STANDARD, nullptr, &pDnsRecord,
                                                nullptr)));
    }

    auto dnsRecordDeleter = [](const PDNS_RECORD &p) { DnsRecordListFree(p, DnsFreeRecordList); };
    std::unique_ptr<DNS_RECORD, decltype(dnsRecordDeleter)> dnsRecords(pDnsRecord, dnsRecordDeleter);

    std::vector<std::string> records;

    while (dnsRecords != nullptr) {
        if (dnsRecords->wType == DNS_TYPE_MX) {
            records.emplace_back(dnsRecords->Data.MX.pNameExchange);
        }

        dnsRecords.reset(dnsRecords->pNext);
    }

    return records;
}

auto TupoSoft::VRF::verify(const std::string &email) -> EmailVerificationData {
    return {};
}
