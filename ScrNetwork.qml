import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: network
    //: dialog title
    title: qsTr("Login to game server")
    standardButtons: Dialog.Cancel | Dialog.Ok
    modal: true
    x: (scrabble3D.width - network.width) / 2
    y: (scrabble3D.height - network.height) / 2
    padding: 12

    ListModel {
        id: countriesModel
        ListElement {country:"Afghanistan";flag:"af.png"}
        ListElement {country:"Albania";flag:"al.png"}
        ListElement {country:"Algeria";flag:"dz.png"}
        ListElement {country:"American Samoa";flag:"as.png"}
        ListElement {country:"Ancient Rome";flag:"latin.png"}
        ListElement {country:"Andorra";flag:"ad.png"}
        ListElement {country:"Angola";flag:"ao.png"}
        ListElement {country:"Anguilla";flag:"ai.png"}
        ListElement {country:"Antarctica";flag:"aq.png"}
        ListElement {country:"Antigua and Barbuda";flag:"ag.png"}
        ListElement {country:"Argentina";flag:"ar.png"}
        ListElement {country:"Armenia";flag:"am.png"}
        ListElement {country:"Aruba";flag:"aw.png"}
        ListElement {country:"Australia";flag:"au.png"}
        ListElement {country:"Austria";flag:"at.png"}
        ListElement {country:"Azerbaijan";flag:"az.png"}
        ListElement {country:"Bahrain";flag:"bh.png"}
        ListElement {country:"Bangladesh";flag:"bd.png"}
        ListElement {country:"Barbados";flag:"bb.png"}
        ListElement {country:"Belarus";flag:"by.png"}
        ListElement {country:"Belgium";flag:"be.png"}
        ListElement {country:"Belize";flag:"bz.png"}
        ListElement {country:"Benin";flag:"bj.png"}
        ListElement {country:"Bermuda";flag:"bm.png"}
        ListElement {country:"Bhutan";flag:"bt.png"}
        ListElement {country:"Bolivia";flag:"bo.png"}
        ListElement {country:"Bosnia and Herzegovina";flag:"ba.png"}
        ListElement {country:"Botswana";flag:"bw.png"}
        ListElement {country:"Bouvet Island";flag:"bv.png"}
        ListElement {country:"Brazil";flag:"br.png"}
        ListElement {country:"British Indian Ocean Territory";flag:"io.png"}
        ListElement {country:"British Virgin Islands";flag:"vg.png"}
        ListElement {country:"Brunei Darussalam";flag:"bn.png"}
        ListElement {country:"Bulgaria";flag:"bg.png"}
        ListElement {country:"Burkina Faso";flag:"bf.png"}
        ListElement {country:"Burundi";flag:"bi.png"}
        ListElement {country:"Cambodia";flag:"kh.png"}
        ListElement {country:"Cameroon";flag:"cm.png"}
        ListElement {country:"Canada";flag:"ca.png"}
        ListElement {country:"Catalonia";flag:"catalonia.png"}
        ListElement {country:"Cape Verde";flag:"cv.png"}
        ListElement {country:"Cayman Islands";flag:"ky.png"}
        ListElement {country:"Central African Republic";flag:"cf.png"}
        ListElement {country:"Chad";flag:"td.png"}
        ListElement {country:"Chile";flag:"cl.png"}
        ListElement {country:"China";flag:"cn.png"}
        ListElement {country:"Christmas Island";flag:"cx.png"}
        ListElement {country:"Cocos (Keeling) Islands";flag:"cc.png"}
        ListElement {country:"Colombia";flag:"co.png"}
        ListElement {country:"Comoros";flag:"km.png"}
        ListElement {country:"Congo";flag:"cg.png"}
        ListElement {country:"Cook Islands";flag:"ck.png"}
        ListElement {country:"Costa Rica";flag:"cr.png"}
        ListElement {country:"Cote D''Ivoire";flag:"ci.png"}
        ListElement {country:"Croatia";flag:"hr.png"}
        ListElement {country:"Cuba";flag:"cu.png"}
        ListElement {country:"Cyprus";flag:"cy.png"}
        ListElement {country:"Czech Republic";flag:"cz.png"}
        ListElement {country:"Denmark";flag:"dk.png"}
        ListElement {country:"Djibouti";flag:"dj.png"}
        ListElement {country:"Dominica";flag:"dm.png"}
        ListElement {country:"Dominican Republic";flag:"do.png"}
        ListElement {country:"East Timor";flag:"tl.png"}
        ListElement {country:"East Timor (Timor-Leste)";flag:"tp.png"}
        ListElement {country:"Ecuador";flag:"ec.png"}
        ListElement {country:"Egypt";flag:"eg.png"}
        ListElement {country:"El Salvador";flag:"sv.png"}
        ListElement {country:"England";flag:"england.png"}
        ListElement {country:"Equatorial Guinea";flag:"gq.png"}
        ListElement {country:"Eritrea";flag:"er.png"}
        ListElement {country:"Estonia";flag:"ee.png"}
        ListElement {country:"Ethiopia";flag:"et.png"}
        ListElement {country:"European Union";flag:"europeanunion.png"}
        ListElement {country:"Falkland Islands";flag:"fk.png"}
        ListElement {country:"Faroe Islands";flag:"fo.png"}
        ListElement {country:"Fiji";flag:"fj.png"}
        ListElement {country:"Finland";flag:"fi.png"}
        ListElement {country:"France";flag:"fr.png"}
        ListElement {country:"French Guiana";flag:"gf.png"}
        ListElement {country:"French Polynesia";flag:"pf.png"}
        ListElement {country:"French Southern Territories";flag:"tf.png"}
        ListElement {country:"Gabon";flag:"ga.png"}
        ListElement {country:"Georgia";flag:"ge.png"}
        ListElement {country:"Germany";flag:"de.png"}
        ListElement {country:"Ghana";flag:"gh.png"}
        ListElement {country:"Gibraltar";flag:"gi.png"}
        ListElement {country:"Greece";flag:"gr.png"}
        ListElement {country:"Greenland";flag:"gl.png"}
        ListElement {country:"Grenada";flag:"gd.png"}
        ListElement {country:"Guadeloupe";flag:"gp.png"}
        ListElement {country:"Guam";flag:"gu.png"}
        ListElement {country:"Guatemala";flag:"gt.png"}
        ListElement {country:"Guernsey and Alderney";flag:"gg.png"}
        ListElement {country:"Guinea";flag:"gn.png"}
        ListElement {country:"Guinea-Bissau";flag:"gw.png"}
        ListElement {country:"Guyana";flag:"gy.png"}
        ListElement {country:"Haiti";flag:"ht.png"}
        ListElement {country:"Heard and McDonald Island";flag:"hm.png"}
        ListElement {country:"Honduras";flag:"hn.png"}
        ListElement {country:"Hong Kong";flag:"hk.png"}
        ListElement {country:"Hungary";flag:"hu.png"}
        ListElement {country:"Iceland";flag:"is.png"}
        ListElement {country:"India";flag:"in.png"}
        ListElement {country:"Indonesia";flag:"id.png"}
        ListElement {country:"Iran";flag:"ir.png"}
        ListElement {country:"Iraq";flag:"iq.png"}
        ListElement {country:"Ireland";flag:"ie.png"}
        ListElement {country:"Isle of Man";flag:"im.png"}
        ListElement {country:"Israel";flag:"il.png"}
        ListElement {country:"Italy";flag:"it.png"}
        ListElement {country:"Jamaica";flag:"jm.png"}
        ListElement {country:"Japan";flag:"jp.png"}
        ListElement {country:"Jersey";flag:"je.png"}
        ListElement {country:"Jordan";flag:"jo.png"}
        ListElement {country:"Kazakhstan";flag:"kz.png"}
        ListElement {country:"Kenya";flag:"ke.png"}
        ListElement {country:"Kiribati";flag:"ki.png"}
        ListElement {country:"Kosovo";flag:"kv.png"}
        ListElement {country:"Kuwait";flag:"kw.png"}
        ListElement {country:"Kyrgyzstan";flag:"kg.png"}
        ListElement {country:"Laos";flag:"la.png"}
        ListElement {country:"Latvia";flag:"lv.png"}
        ListElement {country:"Lebanon";flag:"lb.png"}
        ListElement {country:"Lesotho";flag:"ls.png"}
        ListElement {country:"Liberia";flag:"lr.png"}
        ListElement {country:"Libya";flag:"ly.png"}
        ListElement {country:"Liechtenstein";flag:"li.png"}
        ListElement {country:"Lithuania";flag:"lt.png"}
        ListElement {country:"Luxembourg";flag:"lu.png"}
        ListElement {country:"Macao";flag:"mo.png"}
        ListElement {country:"Macedonia";flag:"mk.png"}
        ListElement {country:"Madagascar";flag:"mg.png"}
        ListElement {country:"Malawi";flag:"mw.png"}
        ListElement {country:"Malaysia";flag:"my.png"}
        ListElement {country:"Maldives";flag:"mv.png"}
        ListElement {country:"Mali";flag:"ml.png"}
        ListElement {country:"Malta";flag:"mt.png"}
        ListElement {country:"Marshall Islands";flag:"mh.png"}
        ListElement {country:"Martinique";flag:"mq.png"}
        ListElement {country:"Mauritania";flag:"mr.png"}
        ListElement {country:"Mauritius";flag:"mu.png"}
        ListElement {country:"Mayotte";flag:"yt.png"}
        ListElement {country:"Mexico";flag:"mx.png"}
        ListElement {country:"Micronesia";flag:"fm.png"}
        ListElement {country:"Moldova";flag:"md.png"}
        ListElement {country:"Monaco";flag:"mc.png"}
        ListElement {country:"Mongolia";flag:"mn.png"}
        ListElement {country:"Montenegro";flag:"cs.png"}
        ListElement {country:"Montserrat";flag:"ms.png"}
        ListElement {country:"Morocco";flag:"ma.png"}
        ListElement {country:"Mozambique";flag:"mz.png"}
        ListElement {country:"Myanmar";flag:"mm.png"}
        ListElement {country:"Namibia";flag:"na.png"}
        ListElement {country:"Nauru";flag:"nr.png"}
        ListElement {country:"Nepal";flag:"np.png"}
        ListElement {country:"Netherlands";flag:"nl.png"}
        ListElement {country:"Netherlands Antilles";flag:"an.png"}
        ListElement {country:"New Caledonia";flag:"nc.png"}
        ListElement {country:"New Zealand";flag:"nz.png"}
        ListElement {country:"Nicaragua";flag:"ni.png"}
        ListElement {country:"Niger";flag:"ne.png"}
        ListElement {country:"Nigeria";flag:"ng.png"}
        ListElement {country:"Niue";flag:"nu.png"}
        ListElement {country:"Norfolk Island";flag:"nf.png"}
        ListElement {country:"North Korea";flag:"kp.png"}
        ListElement {country:"Northern Mariana Islands";flag:"mp.png"}
        ListElement {country:"Norway";flag:"no.png"}
        ListElement {country:"Oman";flag:"om.png"}
        ListElement {country:"Pakistan";flag:"pk.png"}
        ListElement {country:"Palau";flag:"pw.png"}
        ListElement {country:"Palestinian Territory";flag:"ps.png"}
        ListElement {country:"Panama";flag:"pa.png"}
        ListElement {country:"Papua New Guinea";flag:"pg.png"}
        ListElement {country:"Paraguay";flag:"py.png"}
        ListElement {country:"Peru";flag:"pe.png"}
        ListElement {country:"Philippines";flag:"ph.png"}
        ListElement {country:"Pitcairn Island";flag:"pn.png"}
        ListElement {country:"Poland";flag:"pl.png"}
        ListElement {country:"Portugal";flag:"pt.png"}
        ListElement {country:"Puerto Rico";flag:"pr.png"}
        ListElement {country:"Qatar";flag:"qa.png"}
        ListElement {country:"Reunion";flag:"re.png"}
        ListElement {country:"Romania";flag:"ro.png"}
        ListElement {country:"Russia";flag:"ru.png"}
        ListElement {country:"Rwanda";flag:"rw.png"}
        ListElement {country:"Saint Barthelemy";flag:"fr.png"}
        ListElement {country:"Saint Helena";flag:"sh.png"}
        ListElement {country:"Saint Kitts and Nevis";flag:"kn.png"}
        ListElement {country:"Saint Lucia";flag:"lc.png"}
        ListElement {country:"Saint Martin";flag:"fr.png"}
        ListElement {country:"Saint Vincent and the Grenadines";flag:"vc.png"}
        ListElement {country:"Saint-Pierre und Miquelon";flag:"pm.png"}
        ListElement {country:"Samoa";flag:"ws.png"}
        ListElement {country:"San Marino";flag:"sm.png"}
        ListElement {country:"Sao Tome and Principe";flag:"st.png"}
        ListElement {country:"Saudi Arabia";flag:"sa.png"}
        ListElement {country:"Scotland";flag:"scotland.png"}
        ListElement {country:"Senegal";flag:"sn.png"}
        ListElement {country:"Serbia";flag:"rs.png"}
        ListElement {country:"Seychelles";flag:"sc.png"}
        ListElement {country:"Sierra Leone";flag:"sl.png"}
        ListElement {country:"Singapore";flag:"sg.png"}
        ListElement {country:"Slovakia";flag:"sk.png"}
        ListElement {country:"Slovenia";flag:"si.png"}
        ListElement {country:"Solomon Islands";flag:"sb.png"}
        ListElement {country:"Somalia";flag:"so.png"}
        ListElement {country:"South Africa";flag:"za.png"}
        ListElement {country:"South Georgia and the South Sandwich Islands";flag:"gs.png"}
        ListElement {country:"South Korea";flag:"kr.png"}
        ListElement {country:"South Sudan";flag:"ss.png"}
        ListElement {country:"Spain";flag:"es.png"}
        ListElement {country:"Sri Lanka";flag:"lk.png"}
        ListElement {country:"Sudan";flag:"sd.png"}
        ListElement {country:"Suriname";flag:"sr.png"}
        ListElement {country:"Svalbard and Jan Mayen";flag:"sj.png"}
        ListElement {country:"Swaziland";flag:"sz.png"}
        ListElement {country:"Sweden";flag:"se.png"}
        ListElement {country:"Switzerland";flag:"ch.png"}
        ListElement {country:"Syrian Arab Republic";flag:"sy.png"}
        ListElement {country:"Taiwan";flag:"tw.png"}
        ListElement {country:"Tajikistan";flag:"tj.png"}
        ListElement {country:"Tanzania";flag:"tz.png"}
        ListElement {country:"Thailand";flag:"th.png"}
        ListElement {country:"The Bahamas";flag:"bs.png"}
        ListElement {country:"The Gambia";flag:"gm.png"}
        ListElement {country:"Togo";flag:"tg.png"}
        ListElement {country:"Tokelau";flag:"tk.png"}
        ListElement {country:"Tonga";flag:"to.png"}
        ListElement {country:"Trinidad and Tobago";flag:"tt.png"}
        ListElement {country:"Tunisia";flag:"tn.png"}
        ListElement {country:"Turkey";flag:"tr.png"}
        ListElement {country:"Turkmenistan";flag:"tm.png"}
        ListElement {country:"Turks and Caicos Islands";flag:"tc.png"}
        ListElement {country:"Tuvalu";flag:"tv.png"}
        ListElement {country:"Uganda";flag:"ug.png"}
        ListElement {country:"Ukraine";flag:"ua.png"}
        ListElement {country:"United Arab Emirates";flag:"ae.png"}
        ListElement {country:"United Kingdom";flag:"gb.png"}
        ListElement {country:"United States";flag:"us.png"}
        ListElement {country:"United States Minor Outlying Islands";flag:"um.png"}
        ListElement {country:"United States Virgin Islands";flag:"vi.png"}
        ListElement {country:"Uruguay";flag:"uy.png"}
        ListElement {country:"Uzbekistan";flag:"uz.png"}
        ListElement {country:"Vanuatu";flag:"vu.png"}
        ListElement {country:"Vatican";flag:"va.png"}
        ListElement {country:"Venezuela";flag:"ve.png"}
        ListElement {country:"Viet Nam";flag:"vn.png"}
        ListElement {country:"Wales";flag:"wales.png"}
        ListElement {country:"Wallis and Futuna";flag:"wf.png"}
        ListElement {country:"Western Sahara";flag:"eh.png"}
        ListElement {country:"Yemen";flag:"ye.png"}
        ListElement {country:"Zambia";flag:"zm.png"}
        ListElement {country:"Zimbabwe";flag:"zw.png"}
    }

    onAboutToShow: {
        var configData = {}
        configData = GamePlay.loadConfig("")

        name.text = configData["nw_name"]
        password.text = configData["nw_pass"]
        email.text = configData["nw_mail"]
        countryBox.currentIndex = configData["nw_country"]
        city.text = configData["nw_city"]
        network.focus = true
    }

    contentItem: FocusScope {
        id: focus //required to receive return key
        Keys.onReturnPressed: accept()
        focus: standardButton(Dialog.Ok)
        ColumnLayout {
            anchors.fill: parent
            spacing: 6
            Label { text: qsTr("Select the game server you want to connect to:") }
            ComboBox {
                id: server
                Layout.fillWidth: true
                model: ["Free International Scrabble Server"]
                //NOTE network: scrnetwork: make server variable
            }
            Label { text: qsTr("Login name:") }
            TextField {
                id: name
                focus: true
                Layout.fillWidth: true
                KeyNavigation.tab: password
            }
            Label { text: qsTr("Password:") }
            TextInput {
                id: password
                Layout.fillWidth: true
                echoMode: TextInput.Password
                color: palette.buttonText
                KeyNavigation.tab: email
                ToolButton {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    icon.source: "qrc:///resources/eye.png"
                    icon.color: palette.buttonText
                    TapHandler {
                        onPressedChanged: pressed
                             ? password.echoMode = TextInput.Normal
                             : password.echoMode = TextInput.Password
                    }
                }
            }
            Label { text: qsTr("Email address:") }
            TextField {
                id: email
                Layout.fillWidth: true
            }
            GridLayout {
                columns: 2
                Label { text: qsTr("Country:") }
                Label { text: qsTr("City:") }
                ComboBox {
                    id: countryBox
                    Layout.fillWidth: true
                    model: countriesModel
                    textRole: "country"
                    delegate: ItemDelegate {
                        width: countryBox.width
                        contentItem: Row {
                            Image {
                                id: imCountry
                                width: 16
                                source: "qrc:///flags/"+model.flag
                                y: (lbCountry.height - height) / 2
                            }
                            Label {
                                id: lbCountry
                                text: model.country
                                leftPadding: 2
                            }
                        }
                        onClicked: countryBox.currentIndex = index
                    }
                }
                TextField {
                    id: city
                    Layout.fillWidth: true
                }
            }
        }
    }

    onAccepted: {
//        acNetwork.checked = true
        GamePlay.connect(name.text, password.text, email.text, countryBox.currentText, city.text)

        var configData = {}
        configData["nw_name"] = name.text
        configData["nw_pass"] = password.text
        configData["nw_mail"] = email.text
        configData["nw_country"] = countryBox.currentIndex
        configData["nw_city"] = city.text
        GamePlay.saveConfig("", configData)
    }
}
