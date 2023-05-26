#ifndef BWREPORTACTION_H
#define BWREPORTACTION_H

#include <bwformaction.h>
#include <unistd.h>

class BWReportAction: public CGI::BWFormAction {
public:
	BWReportAction();
	~BWReportAction();

	BWKeyPair Exec(const BWString &action, BWKeyPair &data);
};

#endif /* BWREPORTACTION_H */
