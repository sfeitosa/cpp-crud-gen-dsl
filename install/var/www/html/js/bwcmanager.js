$("#bwlogin").click(function() {
    var url = $("#login").attr('action');

    $("#bwerror").empty();

    $("#login").submit();
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

