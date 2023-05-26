#ifndef BWTEMPLATE_H
#define BWTEMPLATE_H

#include <bwlist.h>
#include <bwkeylist.h>
#include <bwkeypair.h>
#include <bwerror.h>
#include <bwstring.h>


class BWTemplate
{
  public:
    BWTemplate();
    BWTemplate(const BWString &s, BWKeyPair &data);
    BWTemplate(BWKeyPair &data);

    ~BWTemplate();

    BWString GetText();
    void SetAttribute(const BWString &key, const BWString &value);
    void ReplaceText(const BWString &key, const BWString &value);
    void SetAttributeList(const BWString &attr, BWList &ls);
    void SetAttributes(BWKeyPair &kpr);
    void SetText(const BWString &text);
    void EnableThrow();
    void DisableThrow();
    void FromFile(const BWString &file);
    void ToFile(const BWString &file);
    void ClearAttributes(void);
    bool HasNullReplacement(void);
    void DisableNullReplacement();
    void EnableNullReplacement();

  protected:
    BWKeyPair bw_data;
    BWString bw_skel;
    bool bw_nullthrow;
    BWString bw_finaltext;
    bool bw_parsed;
    BWKeyPair bw_listlengths;
    bool bw_nullreplacement;
  protected:
    void parseAndReplace();
};

#endif
