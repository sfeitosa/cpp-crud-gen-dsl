$("#bwlogin").click(function() {
    var fdata = $("#login").serialize();
    var url = $("#login").attr('action');

    $("#bwerror").empty();

    $.post(url, fdata, function(data, textStatus, jqXHR) {
        // Se foi retornado algum erro do CGI de login
        if (data.indexOf("bwalert") >= 0) {
            $("#bwerror").html(data);
            $("#bwalert").addClass("in");
        }
        else {
            if (data.indexOf("bwsuccess") >= 0) {
                $("#bwerror").html(data);
                $("#bwsuccess").addClass("in");
                url = $("#bwsuccess").attr("bw-redir");
                window.location.replace("http://" + url + "/bw/cadmin.cgi");
            }
        }
    }).fail(function(jqXHR, textStatus, errorThrown) {
        $("#bwerror").html(errorThrown);
    });           
});

$("#usuario").keypress(function(e) {
    if (e.which == 13) {
        $("#senha").focus();
    }
});

$("#senha").keypress(function(e) {
    if (e.which == 13) {
        $("#bwlogin").trigger('click');
    }
});

