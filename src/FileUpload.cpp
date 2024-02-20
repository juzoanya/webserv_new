/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUpload.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mberline <mberline@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 14:26:14 by mberline          #+#    #+#             */
/*   Updated: 2024/02/19 19:52:16 by mberline         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

const std::string FileUpload::bound = "--";

FileUpload::FileUpload( void )
{ }

FileUpload::~FileUpload( void )
{ }


bool    createFile(std::string const & filePath, const char *start, const char *end)
{
    std::cout << "UPLOAD HERE: " << filePath << " | size: " << end - start << std::endl;
    // std::cout << "CONTENT: $" << std::string(start, end - start) << "$" << std::endl;
    std::ofstream ofs(filePath.c_str(), std::ofstream::binary);
    if (!ofs.is_open()) {
        // std::cout << "ERROR CREATING FILE" << std::endl;
        return (false);
    }
    ofs.write(start, end - start);
    ofs.close();
    return (true);
}

int handleMultipart(std::string const & rootPath, std::string & filename, const char *start, const char *end)
{
    HttpHeader fileHeader;
    int dist = fileHeader.parseHeader(start, end, false);
    std::cout << fileHeader.getHeader("content-disposition") << std::endl;
    std::cout << fileHeader.getHeader("content-type") << std::endl;
    std::string const & contentDisposition = fileHeader.getHeader("content-disposition");
    std::size_t fnamepos = contentDisposition.find("filename="); //error?
    std::cout << "filename: " << filename << std::endl;
    filename = contentDisposition.substr(fnamepos + 10, (contentDisposition.size()) - (fnamepos + 11));
    std::string filePath = rootPath + "/" + filename;
    
    // std::cout << "filename: " << filename << std::endl;
    // std::cout << "FILEDATA: " << std::string(&(*itStart) + dist, &(*itBound)) << std::endl;
    return (createFile(filePath, start + dist, end));
}


bool    FileUpload::parseMultipart( buff_t::const_iterator it_start, buff_t::const_iterator it_end )
{
    // _httpMessage.printMessage();
    // std::cout << "-------- body: \n" << _httpMessage.getBody() << std::endl;
    // exit(1);

    // std::string const & body = _httpMessage.getBody();
    // FileInfos uploadDir(config.getFilePath(), R_OK | W_OK, true);
    // if (uploadDir.getStatus() != ws_http::STATUS_200_OK)
    //     return (processError(config, uploadDir.getStatus()));
    // if (!uploadDir.isDirectory())
    //     return (processError(config, ws_http::STATUS_405_METHOD_NOT_ALLOWED));

    // std::string contentType = _httpMessage.header.getHeader("content-type");
    // std::size_t sepPos1 = contentType.find("multipart/form-data");
    // std::size_t sepPos2 = contentType.find("boundary=");
    // if (sepPos1 != 0 || sepPos2 == std::string::npos)
    //     return (processError(config, ws_http::STATUS_415_UNSUPPORTED_MEDIA_TYPE));
    // std::string const & boundary = contentType.substr(sepPos2 + 9, contentType.size() - sepPos2);
    // std::string boundaryStart = "--";
    // std::string filename, relFilePath;
    // buff_t::const_iterator itStartFile = body.begin() + 2 + boundary.size() + ws_http::crlf.size();
    // buff_t::const_iterator itStart = itStartFile;
    // int fileNbr = 0;
    // for (buff_t::const_iterator itBoundStart = itStartFile; itBoundStart + boundaryStart.size() < body.end(); itStart = itBoundStart + boundaryStart.size()) {
    //     itBoundStart = std::search(itStart, body.end(), boundaryStart.begin(), boundaryStart.end());
    //     if (std::string(itBoundStart + boundaryStart.size(), itBoundStart + boundaryStart.size() + boundary.size()) == boundary) {
    //         handleMultipart(config.getFilePath(), filename, &(*(itStartFile)), &(*itBoundStart));
    //         itStart = itStartFile = itBoundStart + boundaryStart.size() + boundary.size() + ws_http::crlf.size();
    //         fileNbr++;
    //     }
    // }
    // _httpMessage.setResponse()
    // if (fileNbr == 1) {
    //     processResponseFile(config, ws_http::STATUS_303_SEE_OTHER, "GET", _httpMessage.header.getHeader("@path") + "/" + filename, 0);
    //     // _httpMessage.setResponse(ws_http::STATUS_201_CREATED, NULL, "", _httpMessage.header.getHeader("@path") + "/" + filename);
    // } else {
    //     processResponseFile(config, ws_http::STATUS_303_SEE_OTHER, "GET", _httpMessage.header.getHeader("@path"), 0);
    //     // _httpMessage.setResponse(ws_http::STATUS_201_CREATED, NULL, "",  _httpMessage.header.getHeader("@path"));
    // }
}
