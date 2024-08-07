/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConstants.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juzoanya <juzoanya@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/03 11:39:38 by mberline          #+#    #+#             */
/*   Updated: 2024/02/28 21:01:51 by juzoanya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "headers.hpp"

namespace ws_http {

	namespace {
	 
		std::map<const std::string, const std::string> init_mimetypes(void)
		{
			std::map<const std::string, const std::string> httpMimeTypeMap;

			httpMimeTypeMap.insert(std::make_pair("html"      , "text/html"));
			httpMimeTypeMap.insert(std::make_pair("htm"       , "text/html"));
			httpMimeTypeMap.insert(std::make_pair("shtml"     , "text/html"));
			httpMimeTypeMap.insert(std::make_pair("css"       , "text/css"));
			httpMimeTypeMap.insert(std::make_pair("xml"       , "text/xml"));
			httpMimeTypeMap.insert(std::make_pair("md"        , "text/markdown"));
			httpMimeTypeMap.insert(std::make_pair("gif"       , "image/gif"));
			httpMimeTypeMap.insert(std::make_pair("jpeg"      , "image/jpeg"));
			httpMimeTypeMap.insert(std::make_pair("jpg"       , "image/jpeg"));
			httpMimeTypeMap.insert(std::make_pair("js"        , "application/javascript"));
			httpMimeTypeMap.insert(std::make_pair("atom"      , "application/atom+xml"));
			httpMimeTypeMap.insert(std::make_pair("rss"       , "application/rss+xml"));
			httpMimeTypeMap.insert(std::make_pair("mml"       , "text/mathml"));
			httpMimeTypeMap.insert(std::make_pair("txt"       , "text/plain"));
			httpMimeTypeMap.insert(std::make_pair("cpp"       , "text/plain"));
			httpMimeTypeMap.insert(std::make_pair("hpp"       , "text/plain"));
			httpMimeTypeMap.insert(std::make_pair("csv"       , "text/csv"));
			httpMimeTypeMap.insert(std::make_pair("jad"       , "text/vnd.sun.j2me.app-descriptor"));
			httpMimeTypeMap.insert(std::make_pair("wml"       , "text/vnd.wap.wml"));
			httpMimeTypeMap.insert(std::make_pair("htc"       , "text/x-component"));
			httpMimeTypeMap.insert(std::make_pair("avif"      , "image/avif"));
			httpMimeTypeMap.insert(std::make_pair("png"       , "image/png"));
			httpMimeTypeMap.insert(std::make_pair("svg"       , "image/svg+xml"));
			httpMimeTypeMap.insert(std::make_pair("svgz"      , "image/svg+xml"));
			httpMimeTypeMap.insert(std::make_pair("tif"       , "image/tiff"));
			httpMimeTypeMap.insert(std::make_pair("tiff"      , "image/tiff"));
			httpMimeTypeMap.insert(std::make_pair("wbmp"      , "image/vnd.wap.wbmp"));
			httpMimeTypeMap.insert(std::make_pair("webp"      , "image/webp"));
			httpMimeTypeMap.insert(std::make_pair("ico"       , "image/x-icon"));
			httpMimeTypeMap.insert(std::make_pair("jng"       , "image/x-jng"));
			httpMimeTypeMap.insert(std::make_pair("bmp"       , "image/x-ms-bmp"));
			httpMimeTypeMap.insert(std::make_pair("woff"      , "font/woff"));
			httpMimeTypeMap.insert(std::make_pair("woff2"     , "font/woff2"));
			httpMimeTypeMap.insert(std::make_pair("jar"       , "application/java-archive"));
			httpMimeTypeMap.insert(std::make_pair("war"       , "application/java-archive"));
			httpMimeTypeMap.insert(std::make_pair("ear"       , "application/java-archive"));
			httpMimeTypeMap.insert(std::make_pair("json"      , "application/json"));
			httpMimeTypeMap.insert(std::make_pair("hqx"       , "application/mac-binhex40"));
			httpMimeTypeMap.insert(std::make_pair("doc"       , "application/msword"));
			httpMimeTypeMap.insert(std::make_pair("pdf"       , "application/pdf"));
			httpMimeTypeMap.insert(std::make_pair("ps"        , "application/postscript"));
			httpMimeTypeMap.insert(std::make_pair("eps"       , "application/postscript"));
			httpMimeTypeMap.insert(std::make_pair("ai"        , "application/postscript"));
			httpMimeTypeMap.insert(std::make_pair("rtf"       , "application/rtf"));
			httpMimeTypeMap.insert(std::make_pair("m3u8"      , "application/vnd.apple.mpegurl"));
			httpMimeTypeMap.insert(std::make_pair("kml"       , "application/vnd.google-earth.kml+xml"));
			httpMimeTypeMap.insert(std::make_pair("kmz"       , "application/vnd.google-earth.kmz"));
			httpMimeTypeMap.insert(std::make_pair("xls"       , "application/vnd.ms-excel"));
			httpMimeTypeMap.insert(std::make_pair("eot"       , "application/vnd.ms-fontobject"));
			httpMimeTypeMap.insert(std::make_pair("ppt"       , "application/vnd.ms-powerpoint"));
			httpMimeTypeMap.insert(std::make_pair("odg"       , "application/vnd.oasis.opendocument.graphics"));
			httpMimeTypeMap.insert(std::make_pair("odp"       , "application/vnd.oasis.opendocument.presentation"));
			httpMimeTypeMap.insert(std::make_pair("ods"       , "application/vnd.oasis.opendocument.spreadsheet"));
			httpMimeTypeMap.insert(std::make_pair("odt"       , "application/vnd.oasis.opendocument.text"));
			httpMimeTypeMap.insert(std::make_pair("pptx"      , "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
			httpMimeTypeMap.insert(std::make_pair("xlsx"      , "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
			httpMimeTypeMap.insert(std::make_pair("docx"      , "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
			httpMimeTypeMap.insert(std::make_pair("wmlc"      , "application/vnd.wap.wmlc"));
			httpMimeTypeMap.insert(std::make_pair("wasm"      , "application/wasm"));
			httpMimeTypeMap.insert(std::make_pair("7z"        , "application/x-7z-compressed"));
			httpMimeTypeMap.insert(std::make_pair("cco"       , "application/x-cocoa"));
			httpMimeTypeMap.insert(std::make_pair("jardiff"   , "application/x-java-archive-diff"));
			httpMimeTypeMap.insert(std::make_pair("jnlp"      , "application/x-java-jnlp-file"));
			httpMimeTypeMap.insert(std::make_pair("run"       , "application/x-makeself"));
			httpMimeTypeMap.insert(std::make_pair("pl"        , "application/x-perl"));
			httpMimeTypeMap.insert(std::make_pair("pm"        , "application/x-perl"));
			httpMimeTypeMap.insert(std::make_pair("prc"       , "application/x-pilot"));
			httpMimeTypeMap.insert(std::make_pair("pdb"       , "application/x-pilot"));
			httpMimeTypeMap.insert(std::make_pair("rar"       , "application/x-rar-compressed"));
			httpMimeTypeMap.insert(std::make_pair("rpm"       , "application/x-redhat-package-manager"));
			httpMimeTypeMap.insert(std::make_pair("sea"       , "application/x-sea"));
			httpMimeTypeMap.insert(std::make_pair("swf"       , "application/x-shockwave-flash"));
			httpMimeTypeMap.insert(std::make_pair("sit"       , "application/x-stuffit"));
			httpMimeTypeMap.insert(std::make_pair("tcl"       , "application/x-tcl"));
			httpMimeTypeMap.insert(std::make_pair("tk"        , "application/x-tcl"));
			httpMimeTypeMap.insert(std::make_pair("der"       , "application/x-x509-ca-cert"));
			httpMimeTypeMap.insert(std::make_pair("pem"       , "application/x-x509-ca-cert"));
			httpMimeTypeMap.insert(std::make_pair("crt"       , "application/x-x509-ca-cert"));
			httpMimeTypeMap.insert(std::make_pair("xpi"       , "application/x-xpinstall"));
			httpMimeTypeMap.insert(std::make_pair("xhtml"     , "application/xhtml+xml"));
			httpMimeTypeMap.insert(std::make_pair("xspf"      , "application/xspf+xml"));
			httpMimeTypeMap.insert(std::make_pair("zip"       , "application/zip"));
			httpMimeTypeMap.insert(std::make_pair("bin"       , "application/octet-stream"));
			httpMimeTypeMap.insert(std::make_pair("exe"       , "application/octet-stream"));
			httpMimeTypeMap.insert(std::make_pair("dll"       , "application/octet-stream"));
			httpMimeTypeMap.insert(std::make_pair("deb"       , "application/octet-stream"));
			httpMimeTypeMap.insert(std::make_pair("dmg"       , "application/octet-stream"));
			httpMimeTypeMap.insert(std::make_pair("iso"       , "application/octet-stream"));
			httpMimeTypeMap.insert(std::make_pair("img"       , "application/octet-stream"));
			httpMimeTypeMap.insert(std::make_pair("msi"       , "application/octet-stream"));
			httpMimeTypeMap.insert(std::make_pair("msp"       , "application/octet-stream"));
			httpMimeTypeMap.insert(std::make_pair("msm"       , "application/octet-stream"));
			httpMimeTypeMap.insert(std::make_pair("mid"       , "audio/midi"));
			httpMimeTypeMap.insert(std::make_pair("midi"      , "audio/midi"));
			httpMimeTypeMap.insert(std::make_pair("kar"       , "audio/midi"));
			httpMimeTypeMap.insert(std::make_pair("mp3"       , "audio/mpeg"));
			httpMimeTypeMap.insert(std::make_pair("ogg"       , "audio/ogg"));
			httpMimeTypeMap.insert(std::make_pair("m4a"       , "audio/x-m4a"));
			httpMimeTypeMap.insert(std::make_pair("ra"        , "audio/x-realaudio"));
			httpMimeTypeMap.insert(std::make_pair("3gpp"      , "video/3gpp"));
			httpMimeTypeMap.insert(std::make_pair("3gp"       , "video/3gpp"));
			httpMimeTypeMap.insert(std::make_pair("ts"        , "video/mp2t"));
			httpMimeTypeMap.insert(std::make_pair("mp4"       , "video/mp4"));
			httpMimeTypeMap.insert(std::make_pair("mpeg"      , "video/mpeg"));
			httpMimeTypeMap.insert(std::make_pair("mpg"       , "video/mpeg"));
			httpMimeTypeMap.insert(std::make_pair("mov"       , "video/quicktime"));
			httpMimeTypeMap.insert(std::make_pair("webm"      , "video/webm"));
			httpMimeTypeMap.insert(std::make_pair("flv"       , "video/x-flv"));
			httpMimeTypeMap.insert(std::make_pair("m4v"       , "video/x-m4v"));
			httpMimeTypeMap.insert(std::make_pair("mng"       , "video/x-mng"));
			httpMimeTypeMap.insert(std::make_pair("asx"       , "video/x-ms-asf"));
			httpMimeTypeMap.insert(std::make_pair("asf"       , "video/x-ms-asf"));
			httpMimeTypeMap.insert(std::make_pair("wmv"       , "video/x-ms-wmv"));
			httpMimeTypeMap.insert(std::make_pair("avi"       , "video/x-msvideo"));
			httpMimeTypeMap.insert(std::make_pair(""          , "application/octet-stream"));

			return (httpMimeTypeMap);
		}
		
		std::map<const std::string, method_t> init_methods()
		{
			std::map<const std::string, method_t> httpMethodMap;
			httpMethodMap["GET"]        = METHOD_GET;
			httpMethodMap["CONNECT"]    = METHOD_CONNECT;
			httpMethodMap["DELETE"]     = METHOD_DELETE;
			httpMethodMap["HEAD"]       = METHOD_HEAD;
			httpMethodMap["OPTIONS"]    = METHOD_OPTIONS;
			httpMethodMap["PATCH"]      = METHOD_PATCH;
			httpMethodMap["POST"]       = METHOD_POST;
			httpMethodMap["PUT"]        = METHOD_PUT;
			httpMethodMap["TRACE"]      = METHOD_TRACE;
			return (httpMethodMap);
		}

		std::map<const std::string, version_t> init_versions()
		{
			std::map<const std::string, version_t> httpVersionMap;
			httpVersionMap["HTTP/1.0"] = VERSION_1_0;
			httpVersionMap["HTTP/1.1"] = VERSION_1_1;
			httpVersionMap["HTTP/2.0"] = VERSION_2_0;
			httpVersionMap["HTTP/3.0"] = VERSION_3_0;
			return (httpVersionMap);
		}

		std::map<const std::string, statuscodes_t> init_statuscodes(void)
		{
			std::map<const std::string, statuscodes_t> httpStatusCodeMap;

			httpStatusCodeMap["UNDEFINED"]                              = STATUS_UNDEFINED;
			httpStatusCodeMap["200 OK"]                                 = STATUS_200_OK;
			httpStatusCodeMap["201 Created"]                            = STATUS_201_CREATED;
			httpStatusCodeMap["204 No Content"]                         = STATUS_204_NO_CONTENT;
			httpStatusCodeMap["300 Multiple Choices"]                   = STATUS_300_MULTIPLE_CHOICES;
			httpStatusCodeMap["301 Moved Permanently"]                  = STATUS_301_MOVED_PERMANENTLY;
			httpStatusCodeMap["302 Found"]                              = STATUS_302_FOUND;
			httpStatusCodeMap["303 See Other"]                          = STATUS_303_SEE_OTHER;
			httpStatusCodeMap["303 See Other"]                          = STATUS_303_SEE_OTHER;
			httpStatusCodeMap["304 Not Modified"]                       = STATUS_304_NOT_MODIFIED;
			httpStatusCodeMap["305 Use Proxy"]                          = STATUS_305_USE_PROXY;
			httpStatusCodeMap["307 Temporary Redirect"]                 = STATUS_307_TEMPORARY_REDIRECT;
			httpStatusCodeMap["308 Permanent Redirect"]                 = STATUS_308_PERMANENT_REDIRECT;
			httpStatusCodeMap["400 Bad Request"]                        = STATUS_400_BAD_REQUEST;
			httpStatusCodeMap["403 Forbidden"]                          = STATUS_403_FORBIDDEN;
			httpStatusCodeMap["404 Not Found"]                          = STATUS_404_NOT_FOUND;
			httpStatusCodeMap["405 Method Not Allowed"]                 = STATUS_405_METHOD_NOT_ALLOWED;
			httpStatusCodeMap["408 Request Timeout"]                    = STATUS_408_REQUEST_TIMEOUT;
			httpStatusCodeMap["411 Length Required"]                    = STATUS_411_LENGTH_REQUIRED;
			httpStatusCodeMap["413 Payload Too Large"]                  = STATUS_413_PAYLOAD_TOO_LARGE;
			httpStatusCodeMap["414 URI Too Long"]                       = STATUS_414_URI_TOO_LONG;
			httpStatusCodeMap["415 Unsupported Media Type"]             = STATUS_415_UNSUPPORTED_MEDIA_TYPE;
			httpStatusCodeMap["429 Too Many Requests"]                  = STATUS_429_TOO_MANY_REQUESTS;
			httpStatusCodeMap["431 Request Header Fields Too Large"]    = STATUS_431_REQUEST_HEADER_FIELDS_TOO_LARGE;
			httpStatusCodeMap["500 Internal Server Error"]              = STATUS_500_INTERNAL_SERVER_ERROR;
			httpStatusCodeMap["502 Bad Gateway"]						= STATUS_502_BAD_GATEWAY;
			httpStatusCodeMap["501 Not Implemented"]                    = STATUS_501_NOT_IMPLEMENTED;
			httpStatusCodeMap["503 Service Unavailable"]                = STATUS_503_SERVICE_UNAVAILABLE;
			httpStatusCodeMap["504 Gateway Timeout"]					= STATUS_504_GATEWAY_TIMEOUT;
			httpStatusCodeMap["505 HTTP Version Not Supported"]         = STATUS_505_HTTP_VERSION_NOT_SUPPORTED;
			
			return (httpStatusCodeMap);
		}
	}

	bool    CaInCmp::operator()( const std::string& a, const std::string& b ) const
	{
		std::string::const_iterator it_a = a.begin();
		std::string::const_iterator it_b = b.begin();
		for (; (it_a != a.end()) && (it_b != b.end()); ++it_a, (void) ++it_b)
		{
			unsigned char ca = std::tolower(*it_a);
			unsigned char cb = std::tolower(*it_b);
			if (ca < cb)
				return (true);
			if (cb < ca)
				return (false);
		}
		return (it_a == a.end()) && (it_b != b.end());
	}

	const std::string                                           webservVersion = "webserv/0.0.0";
	const std::string                                           headerTchar = "!#$%&'*+-.^_`|~";
	const std::string                                           uriReservedDelims = ":/?#[]@";
	const std::string                                           uriReservedSubDelims = "!$&'()*+,;=";
	const std::string                                           uriUnreserved = "-._~";
	const std::string                                           uriPathOrQueryAllowed = uriUnreserved + uriReservedSubDelims + "@" + ":" + "/";
	const std::string                                           crlf = "\r\n";
	const std::string                                           httpHeaderEnd = crlf + crlf;
	const std::string                                           dummyValue;
	
	const std::map<const std::string, method_t>                 methods_rev = init_methods();
	const std::map<const std::string, version_t>                versions_rev = init_versions();
	const std::map<const std::string, statuscodes_t>            statuscodes_rev = init_statuscodes();
	const std::map<method_t, const std::string>                 methods = ws_http::wsReverseMap(methods_rev);
	const std::map<version_t, const std::string>                versions = ws_http::wsReverseMap(versions_rev);
	const std::map<statuscodes_t, const std::string>            statuscodes = ws_http::wsReverseMap(statuscodes_rev);

	const std::map<const std::string, const std::string>        mimetypes = init_mimetypes();


	std::string getFileExtension( const std::string& filePath ) {
	size_t lastDotPosition = filePath.find_last_of(".");
	if (lastDotPosition != std::string::npos) {
		return (filePath.substr(lastDotPosition + 1));
	}
	return ("");
}

}
