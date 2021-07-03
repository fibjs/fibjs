/*
 * HttpHandler.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "object.h"
#include "HttpHandler.h"
#include "HttpRequest.h"
#include "BufferedStream.h"
#include "JSHandler.h"
#include "ifs/mq.h"
#include "Buffer.h"
#include "MemoryStream.h"
#include "version.h"
#include "ifs/zlib.h"
#include "ifs/console.h"

namespace fibjs {

static const char* s_zipTypes[] = {
    "application/3gpdash-qoe-report+xml",
    "application/3gpp-ims+xml",
    "application/activity+json",
    "application/alto-costmap+json",
    "application/alto-costmapfilter+json",
    "application/alto-directory+json",
    "application/alto-endpointcost+json",
    "application/alto-endpointcostparams+json",
    "application/alto-endpointprop+json",
    "application/alto-endpointpropparams+json",
    "application/alto-error+json",
    "application/alto-networkmap+json",
    "application/alto-networkmapfilter+json",
    "application/atom+xml",
    "application/atomcat+xml",
    "application/atomdeleted+xml",
    "application/atomsvc+xml",
    "application/atsc-dwd+xml",
    "application/atsc-held+xml",
    "application/atsc-rsat+xml",
    "application/auth-policy+xml",
    "application/beep+xml",
    "application/calendar+json",
    "application/calendar+xml",
    "application/ccmp+xml",
    "application/ccxml+xml",
    "application/cdfx+xml",
    "application/cea-2018+xml",
    "application/cellml+xml",
    "application/clue_info+xml",
    "application/cnrp+xml",
    "application/coap-group+json",
    "application/conference-info+xml",
    "application/cpl+xml",
    "application/csta+xml",
    "application/cstadata+xml",
    "application/csvm+json",
    "application/dart",
    "application/dash+xml",
    "application/davmount+xml",
    "application/dialog-info+xml",
    "application/dicom+json",
    "application/dicom+xml",
    "application/dns+json",
    "application/docbook+xml",
    "application/dskpp+xml",
    "application/dssc+xml",
    "application/ecmascript",
    "application/emergencycalldata.comment+xml",
    "application/emergencycalldata.control+xml",
    "application/emergencycalldata.deviceinfo+xml",
    "application/emergencycalldata.providerinfo+xml",
    "application/emergencycalldata.serviceinfo+xml",
    "application/emergencycalldata.subscriberinfo+xml",
    "application/emergencycalldata.veds+xml",
    "application/emma+xml",
    "application/emotionml+xml",
    "application/epp+xml",
    "application/expect-ct-report+json",
    "application/fdt+xml",
    "application/fhir+json",
    "application/fhir+xml",
    "application/fido.trusted-apps+json",
    "application/framework-attributes+xml",
    "application/geo+json",
    "application/geoxacml+xml",
    "application/gml+xml",
    "application/gpx+xml",
    "application/held+xml",
    "application/ibe-key-request+xml",
    "application/ibe-pkg-reply+xml",
    "application/im-iscomposing+xml",
    "application/inkml+xml",
    "application/its+xml",
    "application/javascript",
    "application/jf2feed+json",
    "application/jose+json",
    "application/jrd+json",
    "application/json",
    "application/json-patch+json",
    "application/jsonml+json",
    "application/jwk+json",
    "application/jwk-set+json",
    "application/kpml-request+xml",
    "application/kpml-response+xml",
    "application/ld+json",
    "application/lgr+xml",
    "application/load-control+xml",
    "application/lost+xml",
    "application/lostsync+xml",
    "application/mads+xml",
    "application/manifest+json",
    "application/marcxml+xml",
    "application/mathml+xml",
    "application/mathml-content+xml",
    "application/mathml-presentation+xml",
    "application/mbms-associated-procedure-description+xml",
    "application/mbms-deregister+xml",
    "application/mbms-envelope+xml",
    "application/mbms-msk+xml",
    "application/mbms-msk-response+xml",
    "application/mbms-protection-description+xml",
    "application/mbms-reception-report+xml",
    "application/mbms-register+xml",
    "application/mbms-register-response+xml",
    "application/mbms-schedule+xml",
    "application/mbms-user-service-description+xml",
    "application/media-policy-dataset+xml",
    "application/media_control+xml",
    "application/mediaservercontrol+xml",
    "application/merge-patch+json",
    "application/metalink+xml",
    "application/metalink4+xml",
    "application/mets+xml",
    "application/mmt-aei+xml",
    "application/mmt-usd+xml",
    "application/mods+xml",
    "application/mrb-consumer+xml",
    "application/mrb-publish+xml",
    "application/msc-ivr+xml",
    "application/msc-mixer+xml",
    "application/mud+json",
    "application/nlsml+xml",
    "application/odm+xml",
    "application/oebps-package+xml",
    "application/omdoc+xml",
    "application/p2p-overlay+xml",
    "application/patch-ops-error+xml",
    "application/pidf+xml",
    "application/pidf-diff+xml",
    "application/pls+xml",
    "application/poc-settings+xml",
    "application/postscript",
    "application/ppsp-tracker+json",
    "application/problem+json",
    "application/problem+xml",
    "application/provenance+xml",
    "application/prs.xsf+xml",
    "application/pskc+xml",
    "application/raml+yaml",
    "application/rdap+json",
    "application/rdf+xml",
    "application/reginfo+xml",
    "application/reputon+json",
    "application/resource-lists+xml",
    "application/resource-lists-diff+xml",
    "application/rfc+xml",
    "application/rlmi+xml",
    "application/rls-services+xml",
    "application/route-apd+xml",
    "application/route-s-tsid+xml",
    "application/route-usd+xml",
    "application/rsd+xml",
    "application/rss+xml",
    "application/rtf",
    "application/samlassertion+xml",
    "application/samlmetadata+xml",
    "application/sbml+xml",
    "application/scaip+xml",
    "application/scim+json",
    "application/senml+json",
    "application/senml+xml",
    "application/sensml+json",
    "application/sensml+xml",
    "application/sep+xml",
    "application/shf+xml",
    "application/simple-filter+xml",
    "application/smil+xml",
    "application/soap+xml",
    "application/sparql-results+xml",
    "application/spirits-event+xml",
    "application/srgs+xml",
    "application/sru+xml",
    "application/ssdl+xml",
    "application/ssml+xml",
    "application/stix+json",
    "application/swid+xml",
    "application/tar",
    "application/taxii+json",
    "application/tei+xml",
    "application/thraud+xml",
    "application/tlsrpt+json",
    "application/toml",
    "application/ttml+xml",
    "application/urc-grpsheet+xml",
    "application/urc-ressheet+xml",
    "application/urc-targetdesc+xml",
    "application/urc-uisocketdesc+xml",
    "application/vcard+json",
    "application/vcard+xml",
    "application/vnd.1000minds.decision-model+xml",
    "application/vnd.3gpp-prose+xml",
    "application/vnd.3gpp-prose-pc3ch+xml",
    "application/vnd.3gpp.access-transfer-events+xml",
    "application/vnd.3gpp.bsf+xml",
    "application/vnd.3gpp.gmop+xml",
    "application/vnd.3gpp.mcdata-affiliation-command+xml",
    "application/vnd.3gpp.mcdata-info+xml",
    "application/vnd.3gpp.mcdata-service-config+xml",
    "application/vnd.3gpp.mcdata-ue-config+xml",
    "application/vnd.3gpp.mcdata-user-profile+xml",
    "application/vnd.3gpp.mcptt-affiliation-command+xml",
    "application/vnd.3gpp.mcptt-floor-request+xml",
    "application/vnd.3gpp.mcptt-info+xml",
    "application/vnd.3gpp.mcptt-location-info+xml",
    "application/vnd.3gpp.mcptt-mbms-usage-info+xml",
    "application/vnd.3gpp.mcptt-service-config+xml",
    "application/vnd.3gpp.mcptt-signed+xml",
    "application/vnd.3gpp.mcptt-ue-config+xml",
    "application/vnd.3gpp.mcptt-ue-init-config+xml",
    "application/vnd.3gpp.mcptt-user-profile+xml",
    "application/vnd.3gpp.mcvideo-affiliation-command+xml",
    "application/vnd.3gpp.mcvideo-affiliation-info+xml",
    "application/vnd.3gpp.mcvideo-info+xml",
    "application/vnd.3gpp.mcvideo-location-info+xml",
    "application/vnd.3gpp.mcvideo-mbms-usage-info+xml",
    "application/vnd.3gpp.mcvideo-service-config+xml",
    "application/vnd.3gpp.mcvideo-transmission-request+xml",
    "application/vnd.3gpp.mcvideo-ue-config+xml",
    "application/vnd.3gpp.mcvideo-user-profile+xml",
    "application/vnd.3gpp.mid-call+xml",
    "application/vnd.3gpp.sms+xml",
    "application/vnd.3gpp.srvcc-ext+xml",
    "application/vnd.3gpp.srvcc-info+xml",
    "application/vnd.3gpp.state-and-event-info+xml",
    "application/vnd.3gpp.ussd+xml",
    "application/vnd.3gpp2.bcmcsinfo+xml",
    "application/vnd.adobe.xdp+xml",
    "application/vnd.amadeus+json",
    "application/vnd.amundsen.maze+xml",
    "application/vnd.api+json",
    "application/vnd.apothekende.reservation+json",
    "application/vnd.apple.installer+xml",
    "application/vnd.artisan+json",
    "application/vnd.avalon+json",
    "application/vnd.avistar+xml",
    "application/vnd.balsamiq.bmml+xml",
    "application/vnd.bbf.usp.msg+json",
    "application/vnd.bekitzur-stech+json",
    "application/vnd.biopax.rdf+xml",
    "application/vnd.byu.uapi+json",
    "application/vnd.capasystems-pg+json",
    "application/vnd.chemdraw+xml",
    "application/vnd.citationstyles.style+xml",
    "application/vnd.collection+json",
    "application/vnd.collection.doc+json",
    "application/vnd.collection.next+json",
    "application/vnd.coreos.ignition+json",
    "application/vnd.criticaltools.wbs+xml",
    "application/vnd.cryptii.pipe+json",
    "application/vnd.ctct.ws+xml",
    "application/vnd.cyan.dean.root+xml",
    "application/vnd.dart",
    "application/vnd.datapackage+json",
    "application/vnd.dataresource+json",
    "application/vnd.dece.ttml+xml",
    "application/vnd.dm.delegation+xml",
    "application/vnd.document+json",
    "application/vnd.drive+json",
    "application/vnd.dvb.notif-aggregate-root+xml",
    "application/vnd.dvb.notif-container+xml",
    "application/vnd.dvb.notif-generic+xml",
    "application/vnd.dvb.notif-ia-msglist+xml",
    "application/vnd.dvb.notif-ia-registration-request+xml",
    "application/vnd.dvb.notif-ia-registration-response+xml",
    "application/vnd.dvb.notif-init+xml",
    "application/vnd.emclient.accessrequest+xml",
    "application/vnd.eprints.data+xml",
    "application/vnd.eszigno3+xml",
    "application/vnd.etsi.aoc+xml",
    "application/vnd.etsi.cug+xml",
    "application/vnd.etsi.iptvcommand+xml",
    "application/vnd.etsi.iptvdiscovery+xml",
    "application/vnd.etsi.iptvprofile+xml",
    "application/vnd.etsi.iptvsad-bc+xml",
    "application/vnd.etsi.iptvsad-cod+xml",
    "application/vnd.etsi.iptvsad-npvr+xml",
    "application/vnd.etsi.iptvservice+xml",
    "application/vnd.etsi.iptvsync+xml",
    "application/vnd.etsi.iptvueprofile+xml",
    "application/vnd.etsi.mcid+xml",
    "application/vnd.etsi.overload-control-policy-dataset+xml",
    "application/vnd.etsi.pstn+xml",
    "application/vnd.etsi.sci+xml",
    "application/vnd.etsi.simservs+xml",
    "application/vnd.etsi.tsl+xml",
    "application/vnd.futoin+json",
    "application/vnd.geo+json",
    "application/vnd.geocube+xml",
    "application/vnd.google-earth.kml+xml",
    "application/vnd.gov.sk.e-form+xml",
    "application/vnd.gov.sk.xmldatacontainer+xml",
    "application/vnd.hal+json",
    "application/vnd.hal+xml",
    "application/vnd.handheld-entertainment+xml",
    "application/vnd.hc+json",
    "application/vnd.heroku+json",
    "application/vnd.hyper+json",
    "application/vnd.hyper-item+json",
    "application/vnd.hyperdrive+json",
    "application/vnd.ims.lis.v2.result+json",
    "application/vnd.ims.lti.v2.toolconsumerprofile+json",
    "application/vnd.ims.lti.v2.toolproxy+json",
    "application/vnd.ims.lti.v2.toolproxy.id+json",
    "application/vnd.ims.lti.v2.toolsettings+json",
    "application/vnd.ims.lti.v2.toolsettings.simple+json",
    "application/vnd.informedcontrol.rms+xml",
    "application/vnd.infotech.project+xml",
    "application/vnd.iptc.g2.catalogitem+xml",
    "application/vnd.iptc.g2.conceptitem+xml",
    "application/vnd.iptc.g2.knowledgeitem+xml",
    "application/vnd.iptc.g2.newsitem+xml",
    "application/vnd.iptc.g2.newsmessage+xml",
    "application/vnd.iptc.g2.packageitem+xml",
    "application/vnd.iptc.g2.planningitem+xml",
    "application/vnd.irepository.package+xml",
    "application/vnd.las.las+json",
    "application/vnd.las.las+xml",
    "application/vnd.leap+json",
    "application/vnd.liberty-request+xml",
    "application/vnd.llamagraphics.life-balance.exchange+xml",
    "application/vnd.marlin.drm.actiontoken+xml",
    "application/vnd.marlin.drm.conftoken+xml",
    "application/vnd.marlin.drm.license+xml",
    "application/vnd.mason+json",
    "application/vnd.micro+json",
    "application/vnd.miele+json",
    "application/vnd.mozilla.xul+xml",
    "application/vnd.ms-fontobject",
    "application/vnd.ms-office.activex+xml",
    "application/vnd.ms-opentype",
    "application/vnd.ms-playready.initiator+xml",
    "application/vnd.ms-printdevicecapabilities+xml",
    "application/vnd.ms-printing.printticket+xml",
    "application/vnd.ms-printschematicket+xml",
    "application/vnd.nearst.inv+json",
    "application/vnd.nokia.conml+xml",
    "application/vnd.nokia.iptv.config+xml",
    "application/vnd.nokia.landmark+xml",
    "application/vnd.nokia.landmarkcollection+xml",
    "application/vnd.nokia.n-gage.ac+xml",
    "application/vnd.nokia.pcd+xml",
    "application/vnd.oftn.l10n+json",
    "application/vnd.oipf.contentaccessdownload+xml",
    "application/vnd.oipf.contentaccessstreaming+xml",
    "application/vnd.oipf.dae.svg+xml",
    "application/vnd.oipf.dae.xhtml+xml",
    "application/vnd.oipf.mippvcontrolmessage+xml",
    "application/vnd.oipf.spdiscovery+xml",
    "application/vnd.oipf.spdlist+xml",
    "application/vnd.oipf.ueprofile+xml",
    "application/vnd.oipf.userprofile+xml",
    "application/vnd.oma.bcast.associated-procedure-parameter+xml",
    "application/vnd.oma.bcast.drm-trigger+xml",
    "application/vnd.oma.bcast.imd+xml",
    "application/vnd.oma.bcast.notification+xml",
    "application/vnd.oma.bcast.sgdd+xml",
    "application/vnd.oma.bcast.smartcard-trigger+xml",
    "application/vnd.oma.bcast.sprov+xml",
    "application/vnd.oma.cab-address-book+xml",
    "application/vnd.oma.cab-feature-handler+xml",
    "application/vnd.oma.cab-pcc+xml",
    "application/vnd.oma.cab-subs-invite+xml",
    "application/vnd.oma.cab-user-prefs+xml",
    "application/vnd.oma.dd2+xml",
    "application/vnd.oma.drm.risd+xml",
    "application/vnd.oma.group-usage-list+xml",
    "application/vnd.oma.lwm2m+json",
    "application/vnd.oma.pal+xml",
    "application/vnd.oma.poc.detailed-progress-report+xml",
    "application/vnd.oma.poc.final-report+xml",
    "application/vnd.oma.poc.groups+xml",
    "application/vnd.oma.poc.invocation-descriptor+xml",
    "application/vnd.oma.poc.optimized-progress-report+xml",
    "application/vnd.oma.scidm.messages+xml",
    "application/vnd.oma.xcap-directory+xml",
    "application/vnd.omads-email+xml",
    "application/vnd.omads-file+xml",
    "application/vnd.omads-folder+xml",
    "application/vnd.openblox.game+xml",
    "application/vnd.openstreetmap.data+xml",
    "application/vnd.openxmlformats-officedocument.custom-properties+xml",
    "application/vnd.openxmlformats-officedocument.customxmlproperties+xml",
    "application/vnd.openxmlformats-officedocument.drawing+xml",
    "application/vnd.openxmlformats-officedocument.drawingml.chart+xml",
    "application/vnd.openxmlformats-officedocument.drawingml.chartshapes+xml",
    "application/vnd.openxmlformats-officedocument.drawingml.diagramcolors+xml",
    "application/vnd.openxmlformats-officedocument.drawingml.diagramdata+xml",
    "application/vnd.openxmlformats-officedocument.drawingml.diagramlayout+xml",
    "application/vnd.openxmlformats-officedocument.drawingml.diagramstyle+xml",
    "application/vnd.openxmlformats-officedocument.extended-properties+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.commentauthors+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.comments+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.handoutmaster+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.notesmaster+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.notesslide+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.presentation.main+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.presprops+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.slide+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.slidelayout+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.slidemaster+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.slideshow.main+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.slideupdateinfo+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.tablestyles+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.tags+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.template.main+xml",
    "application/vnd.openxmlformats-officedocument.presentationml.viewprops+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.calcchain+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.chartsheet+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.comments+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.connections+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.dialogsheet+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.externallink+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.pivotcachedefinition+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.pivotcacherecords+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.pivottable+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.querytable+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.revisionheaders+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.revisionlog+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.sharedstrings+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheetmetadata+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.table+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.tablesinglecells+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.template.main+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.usernames+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.volatiledependencies+xml",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml",
    "application/vnd.openxmlformats-officedocument.theme+xml",
    "application/vnd.openxmlformats-officedocument.themeoverride+xml",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.comments+xml",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.document.glossary+xml",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.endnotes+xml",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.fonttable+xml",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.footer+xml",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.footnotes+xml",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.numbering+xml",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.settings+xml",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.template.main+xml",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.websettings+xml",
    "application/vnd.openxmlformats-package.core-properties+xml",
    "application/vnd.openxmlformats-package.digital-signature-xmlsignature+xml",
    "application/vnd.openxmlformats-package.relationships+xml",
    "application/vnd.oracle.resource+json",
    "application/vnd.otps.ct-kip+xml",
    "application/vnd.pagerduty+json",
    "application/vnd.poc.group-advertisement+xml",
    "application/vnd.pwg-xhtml-print+xml",
    "application/vnd.radisys.moml+xml",
    "application/vnd.radisys.msml+xml",
    "application/vnd.radisys.msml-audit+xml",
    "application/vnd.radisys.msml-audit-conf+xml",
    "application/vnd.radisys.msml-audit-conn+xml",
    "application/vnd.radisys.msml-audit-dialog+xml",
    "application/vnd.radisys.msml-audit-stream+xml",
    "application/vnd.radisys.msml-conf+xml",
    "application/vnd.radisys.msml-dialog+xml",
    "application/vnd.radisys.msml-dialog-base+xml",
    "application/vnd.radisys.msml-dialog-fax-detect+xml",
    "application/vnd.radisys.msml-dialog-fax-sendrecv+xml",
    "application/vnd.radisys.msml-dialog-group+xml",
    "application/vnd.radisys.msml-dialog-speech+xml",
    "application/vnd.radisys.msml-dialog-transform+xml",
    "application/vnd.recordare.musicxml+xml",
    "application/vnd.restful+json",
    "application/vnd.route66.link66+xml",
    "application/vnd.shootproof+json",
    "application/vnd.shopkick+json",
    "application/vnd.siren+json",
    "application/vnd.software602.filler.form+xml",
    "application/vnd.solent.sdkm+xml",
    "application/vnd.sun.wadl+xml",
    "application/vnd.syncml+xml",
    "application/vnd.syncml.dm+xml",
    "application/vnd.syncml.dmddf+xml",
    "application/vnd.syncml.dmtnds+xml",
    "application/vnd.tableschema+json",
    "application/vnd.think-cell.ppttc+json",
    "application/vnd.tmd.mediaflex.api+xml",
    "application/vnd.uoml+xml",
    "application/vnd.vel+json",
    "application/vnd.wv.csp+xml",
    "application/vnd.wv.ssp+xml",
    "application/vnd.xacml+json",
    "application/vnd.xmi+xml",
    "application/vnd.yamaha.openscoreformat.osfpvg+xml",
    "application/vnd.zzazz.deck+xml",
    "application/voicexml+xml",
    "application/voucher-cms+json",
    "application/wasm",
    "application/watcherinfo+xml",
    "application/webpush-options+json",
    "application/wsdl+xml",
    "application/wspolicy+xml",
    "application/x-dtbncx+xml",
    "application/x-dtbook+xml",
    "application/x-dtbresource+xml",
    "application/x-httpd-php",
    "application/x-javascript",
    "application/x-ns-proxy-autoconfig",
    "application/x-sh",
    "application/x-tar",
    "application/x-virtualbox-hdd",
    "application/x-virtualbox-ova",
    "application/x-virtualbox-ovf",
    "application/x-virtualbox-vbox",
    "application/x-virtualbox-vdi",
    "application/x-virtualbox-vhd",
    "application/x-virtualbox-vmdk",
    "application/x-web-app-manifest+json",
    "application/x-www-form-urlencoded",
    "application/x-xliff+xml",
    "application/xacml+xml",
    "application/xaml+xml",
    "application/xcap-att+xml",
    "application/xcap-caps+xml",
    "application/xcap-diff+xml",
    "application/xcap-el+xml",
    "application/xcap-error+xml",
    "application/xcap-ns+xml",
    "application/xcon-conference-info+xml",
    "application/xcon-conference-info-diff+xml",
    "application/xenc+xml",
    "application/xhtml+xml",
    "application/xhtml-voice+xml",
    "application/xliff+xml",
    "application/xml",
    "application/xml-dtd",
    "application/xml-patch+xml",
    "application/xmpp+xml",
    "application/xop+xml",
    "application/xproc+xml",
    "application/xslt+xml",
    "application/xspf+xml",
    "application/xv+xml",
    "application/yang-data+json",
    "application/yang-data+xml",
    "application/yang-patch+json",
    "application/yang-patch+xml",
    "application/yin+xml",
    "font/otf",
    "font/ttf",
    "image/bmp",
    "image/svg+xml",
    "image/vnd.adobe.photoshop",
    "image/x-icon",
    "image/x-ms-bmp",
    "message/imdn+xml",
    "message/rfc822",
    "model/gltf+json",
    "model/gltf-binary",
    "model/vnd.collada+xml",
    "model/vnd.moml+xml",
    "model/x3d+xml",
    "x-shader/x-fragment",
    "x-shader/x-vertex"
};

static int32_t mt_cmp(const void* p, const void* q)
{
    return qstricmp(*(const char**)p, *(const char**)q);
}

result_t HttpHandler_base::_new(Handler_base* hdlr, obj_ptr<HttpHandler_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<HttpHandler> ht_hdlr = new HttpHandler();
    ht_hdlr->wrap(This);
    ht_hdlr->set_handler(hdlr);

    retVal = ht_hdlr;

    return 0;
}

HttpHandler::HttpHandler()
    : m_crossDomain(false)
    , m_maxHeadersCount(128)
    , m_maxBodySize(64)
    , m_enableEncoding(false)
{
    m_serverName = "fibjs/";
    m_serverName.append(fibjs_version);
}

result_t HttpHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
    AsyncEvent* ac)
{
    class asyncInvoke : public AsyncState {
    public:
        asyncInvoke(HttpHandler* pThis, Stream_base* stm, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_stm(stm)
            , m_options(false)
        {
            m_stmBuffered = new BufferedStream(stm);
            m_stmBuffered->set_EOL("\r\n");

            m_req = new HttpRequest();
            m_req->get_response(m_rep);

            m_req->set_maxHeadersCount(pThis->m_maxHeadersCount);
            m_req->set_maxBodySize(pThis->m_maxBodySize);

            next(read);
        }

        ON_STATE(asyncInvoke, read)
        {
            bool bKeepAlive = false;

            m_rep->get_keepAlive(bKeepAlive);

            if (!bKeepAlive)
                return next(CALL_RETURN_NULL);

            m_options = false;

            m_zip.Release();
            m_body.Release();

            m_req->clear();
            return m_req->readFrom(m_stmBuffered, next(invoke));
        }

        ON_STATE(asyncInvoke, invoke)
        {
            if (n == CALL_RETURN_NULL)
                return next(CALL_RETURN_NULL);

            exlib::string str;

            m_req->get_protocol(str);
            m_rep->set_protocol(str);

            m_rep->addHeader("Server", m_pThis->m_serverName);

            bool bKeepAlive;

            m_req->get_keepAlive(bKeepAlive);
            m_rep->set_keepAlive(bKeepAlive);

            m_d.now();

            if (m_pThis->m_crossDomain) {
                m_req->get_address(str);

                exlib::string origin;

                if (m_req->firstHeader("origin", origin)
                    != CALL_RETURN_NULL) {
                    m_rep->setHeader("Access-Control-Allow-Credentials", "true");
                    m_rep->setHeader("Access-Control-Allow-Origin", origin);

                    m_req->get_method(str);

                    if (!qstricmp(str.c_str(), "options")) {
                        m_options = true;

                        m_rep->setHeader("Access-Control-Allow-Methods", "*");
                        m_rep->setHeader("Access-Control-Allow-Headers",
                            m_pThis->m_allowHeaders);
                        m_rep->setHeader("Access-Control-Max-Age", "1728000");

                        return next(send);
                    }
                }
            }

            return mq_base::invoke(m_pThis->m_hdlr, m_req, next(send));
        }

        ON_STATE(asyncInvoke, send)
        {
            int32_t s;
            bool t = false;
            date_t d;

            d.now();

            m_rep->get_statusCode(s);
            if (s == 200 && !m_options) {
                m_rep->hasHeader("Last-Modified", t);
                if (!t) {
                    m_rep->addHeader("Cache-Control", "no-cache, no-store");
                    m_rep->addHeader("Expires", "-1");
                }
            }

            exlib::string str;

            m_req->get_method(str);
            bool headOnly = !qstricmp(str.c_str(), "head");

            if (headOnly) {
                m_rep->set_keepAlive(false);
                return m_rep->sendHeader(m_stm, next(end));
            }

            int64_t len;

            m_rep->get_length(len);

            if (m_pThis->m_enableEncoding && len > 128 && len < 1024 * 1024 * 64) {
                exlib::string hdr;

                if (m_req->firstHeader("Accept-Encoding", hdr) != CALL_RETURN_NULL) {
                    int32_t type = 0;

                    if (qstristr(hdr.c_str(), "gzip"))
                        type = 1;
                    else if (qstristr(hdr.c_str(), "deflate"))
                        type = 2;

                    if (type != 0) {
                        if (m_rep->firstHeader("Content-Type", hdr) != CALL_RETURN_NULL) {
                            const char* pKey = hdr.c_str();
                            if (qstricmp(hdr.c_str(), "text/", 5)
                                && !bsearch(&pKey, &s_zipTypes, ARRAYSIZE(s_zipTypes),
                                    sizeof(pKey), mt_cmp))
                                type = 0;
                        } else
                            type = 0;
                    }

                    if (type != 0) {
                        if (m_rep->firstHeader("Content-Encoding", hdr) != CALL_RETURN_NULL)
                            type = 0;
                    }

                    if (type != 0) {
                        m_rep->addHeader("Content-Encoding", type == 1 ? "gzip" : "deflate");

                        m_rep->get_body(m_body);
                        m_body->rewind();

                        m_zip = new MemoryStream();

                        if (type == 1)
                            return zlib_base::gzipTo(m_body, m_zip, next(zip));
                        else
                            return zlib_base::deflateTo(m_body, m_zip, -1, next(zip));
                    }
                }
            }

            return m_rep->sendTo(m_stm, next(end));
        }

        ON_STATE(asyncInvoke, zip)
        {
            m_rep->set_body(m_zip);
            return m_rep->sendTo(m_stm, next(end));
        }

        ON_STATE(asyncInvoke, end)
        {
            if (!m_body)
                m_rep->get_body(m_body);

            if (!m_body)
                return next(read);

            return m_body->close(next(read));
        }

        virtual int32_t error(int32_t v)
        {
            if (at(invoke)) {
                exlib::string err = getResultMessage(v);

                m_req->set_lastError(err);
                errorLog("HttpHandler: " + err);

                m_rep->set_statusCode(500);
                return 0;
            }

            if (at(read)) {
                m_rep->set_keepAlive(false);
                m_rep->set_statusCode(400);
                next(send);
                m_d.now();
                return 0;
            }

            return next(CALL_RETURN_NULL);
        }

    private:
        obj_ptr<HttpHandler> m_pThis;
        obj_ptr<Stream_base> m_stm;
        obj_ptr<BufferedStream_base> m_stmBuffered;
        obj_ptr<HttpRequest_base> m_req;
        obj_ptr<HttpResponse_base> m_rep;
        obj_ptr<MemoryStream> m_zip;
        obj_ptr<SeekableStream_base> m_body;
        date_t m_d;
        bool m_options;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Stream_base> stm = Stream_base::getInstance(v);
    if (stm == NULL) {
        obj_ptr<Message_base> msg = Message_base::getInstance(v);
        if (msg != NULL)
            msg->get_stream(stm);
    }

    if (stm == NULL)
        return CHECK_ERROR(CALL_E_BADVARTYPE);

    return (new asyncInvoke(this, stm, ac))->post(0);
}

result_t HttpHandler::enableCrossOrigin(exlib::string allowHeaders)
{
    m_crossDomain = true;
    m_allowHeaders = allowHeaders;
    return 0;
}

result_t HttpHandler::get_maxHeadersCount(int32_t& retVal)
{
    retVal = m_maxHeadersCount;
    return 0;
}

result_t HttpHandler::set_maxHeadersCount(int32_t newVal)
{
    if (newVal < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    m_maxHeadersCount = newVal;
    return 0;
}

result_t HttpHandler::get_maxBodySize(int32_t& retVal)
{
    retVal = m_maxBodySize;
    return 0;
}

result_t HttpHandler::set_maxBodySize(int32_t newVal)
{
    m_maxBodySize = newVal;
    return 0;
}

result_t HttpHandler::get_enableEncoding(bool& retVal)
{
    retVal = m_enableEncoding;
    return 0;
}

result_t HttpHandler::set_enableEncoding(bool newVal)
{
    m_enableEncoding = newVal;
    return 0;
}

result_t HttpHandler::get_serverName(exlib::string& retVal)
{
    retVal = m_serverName;
    return 0;
}

result_t HttpHandler::set_serverName(exlib::string newVal)
{
    m_serverName = newVal;
    return 0;
}

result_t HttpHandler::get_handler(obj_ptr<Handler_base>& retVal)
{
    retVal = m_hdlr;
    return 0;
}

result_t HttpHandler::set_handler(Handler_base* newVal)
{
    SetPrivate("handler", newVal->wrap());
    m_hdlr = newVal;

    return 0;
}
}
