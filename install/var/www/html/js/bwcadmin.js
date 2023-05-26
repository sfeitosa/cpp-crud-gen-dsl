// Variáveis Globais
// -----------------
var timeoutHandler;

$( document ).ready(function() {
    $(".container").on("click", "a.bwlink", function(event) {
        event.preventDefault();
        var href = $(this).attr('href');
        var pdata = "curpage=1";
        var reloadTime = 0;

        clearTimeout(timeoutHandler);

        $.post(href, pdata, function(data) {
            $("#bwpanel").html(data);
            loadPaginator(href);

            reloadTime = $("#bw-table-list").attr("bw-reload-time");

            if (reloadTime > 0) {
                reloadPanel(href, reloadTime);
            }
        });
    });

    /*
     * Função responsável por tratar as requisições solicitadas pelas
     * opcoes das tabelas  
     */
    $(".container").on("click", "a.bwoption", function(event) {
        event.preventDefault();
        var href = $(this).attr('href');

        $("#bw-dialog-message").empty();

        $.post(href, function(data) {
            $("#bwpanel").html(data);
            $("#bw-dialog-message").addClass("in");
            loadPaginator(href);
       });
    });

    /*
     * Função responsável por invocar formulários em formato Modal
     */
    $(".container").on("click", "a.bwmodal", function(event) {
        event.preventDefault();
        var href = $(this).attr('href');

        clearTimeout(timeoutHandler);

        $("#bw-dialog-message").empty();

        $.post(href, function(data) {
            if (data.indexOf("bw-error-msg") >= 0) {
                $("#bw-dialog-message").html(data);
                $("#bw-dialog-message").addClass("in");
            }
            else {
                $("#bwmodal").html(data);
                $("#bwmodal").modal("show");
                loadDynamicFields();
            }
        });
    });

    /*
     * Função para tratar o evento "Salvar"
     */
    $(".container").on("click", "button.bwsave", function(event) {
        var action = $("#bwform").attr("action");
        var fdata = $("#bwform").serialize();
        var reloadTime = 0;
        var ref = this;
        var btnText = $(this).html();

        $(this).attr("disabled", "disabled");
        $(this).html("<span><img src='/beanweb/img/loading.gif'></span> Processando...");

        $.post(action, fdata, function(data) {
            if (data.indexOf("bw-error-msg") >= 0) {
                $(ref).removeAttr("disabled");
                $(ref).html(btnText);
                $("#bw-form-error").html(data);
                $("#bw-form-error").addClass("in");
            }
            else {
                $("#bwmodal").modal("hide");
                $("#bwpanel").html(data);
                $("#bw-dialog-message").addClass("in");
                loadPaginator(action);

                reloadTime = $("#bw-table-list").attr("bw-reload-time");

                if (reloadTime > 0) {
                    reloadPanel(action, reloadTime);
                }
            }
       });
    });

    /*
     * Função para tratar o evento "Move Left" do campo InputToList
     */
    $(".container").on("click", ".bw-il-left", function(event) {
        var fieldname = $(this).attr("fieldname");
        var srcvalue = $("#txt_" + fieldname + "_src").val();

        if (srcvalue.length > 0) {
            if ( $("#sel_" + fieldname + "_dst option[value='" + srcvalue + "']").length > 0 ) {
                alert("Este campo já está cadastrado!");
            }
            else {
                $("#sel_" + fieldname + "_dst").append("<option value='" + srcvalue + "'>" + srcvalue + "</option>");
                $("#txt_" + fieldname + "_src").val("");
            }
        }
        $("#txt_" + fieldname + "_src").focus();

        updateList(fieldname);
    });

    /*
     * Função para tratar o evento "Remove" do campo InputToList
     */
    $(".container").on("click", ".bw-il-remove", function(event) {
        var fieldname = $(this).attr("fieldname");
        $("#sel_" + fieldname + "_dst option:selected").remove();
        $("#txt_" + fieldname + "_src").focus();

        updateList(fieldname);
    });

    /*
     * Função para tratar o evento "Adicionar" do campo ListToList
     */
    $(".container").on("click", ".bw-ll-left", function(event) {
        var fieldname = $(this).attr("fieldname");

        $("#sel_" + fieldname + "_src option:selected").each(function() {
            $("#sel_" + fieldname + "_dst").append("<option value='" + this.value + "'>" + this.text + "</option>");
        });

        $("#sel_" + fieldname + "_src option:selected").remove();
        $("#sel_" + fieldname + "_src").focus();

        updateList(fieldname);
    });

    /*
     * Função para tratar o evento "Remover" do campo ListToList
     */
    $(".container").on("click", ".bw-ll-right", function(event) {
        var fieldname = $(this).attr("fieldname");

        $("#sel_" + fieldname + "_dst option:selected").each(function() {
            $("#sel_" + fieldname + "_src").append("<option value='" + this.value + "'>" + this.text + "</option>");
        });

        $("#sel_" + fieldname + "_dst option:selected").remove();
        $("#sel_" + fieldname + "_dst").focus();

        updateList(fieldname);
    });

    /*
     * Função para tratar o evento "Click" dos campos RadioList
     */
    $(".container").on("click", ".bw-rl-select", function(event) {
        var fieldname = $(this).attr("fieldname");
        var value = $(this).attr("value");
        $("#" + fieldname).val(value);
    });

    /*
     * Função para tratar o evento "Click" dos campos CheckBoxList
     */
    $(".container").on("click", ".bw-cbl-select", function(event) {
        var fieldname = $(this).attr("fieldname");
        updateCheckBoxList(fieldname);
    });

    /*
     * Código responsável por organizar os sub-menus da barra de navegação
     */
    $('ul.dropdown-menu [data-toggle=dropdown]').on('click', function(event) {
        // Avoid following the href location when clicking
        event.preventDefault(); 
        // Avoid having the menu to close when clicking
        event.stopPropagation(); 
        // If a menu is already open we close it
        $('ul.dropdown-menu [data-toggle=dropdown]').parent().removeClass('open');
        // opening the one you clicked on
        $(this).parent().addClass('open');

        var menu = $(this).parent().find("ul");
        var menupos = menu.offset();
  
        if ((menupos.left + menu.width()) + 30 > $(window).width()) {
            var newpos = - menu.width();      
        } else {
            var newpos = $(this).parent().width();
        }
        menu.css({ left:newpos });
    });
    /*
     * Fim do código de tratamento do MENU
     */



});

function loadDynamicFields() {
    // Carregando o DatePicker - ver uma forma melhor de fazer isso!!!
    $('div.bfh-datepicker').each(function() {
        var $datepicker;
        $datepicker = $(this);
        $datepicker.bfhdatepicker($datepicker.data());
    });

    // Carregando o TimePicker - ver uma forma melhor de fazer isso!!!
    $('div.bfh-timepicker').each(function() {
       var $timepicker;
       $timepicker = $(this);
       $timepicker.bfhtimepicker($timepicker.data());
    });

    // Carregando valores "default" dos campos InputToList
    $('.bw-il-autoload').each(function() {
        var field, values, list;

        field = $(this).attr('name');
        values = $(this).attr('value');

        if (values != "") {
            list = values.split(",");
            
            for (i = 0; i < list.length; i++) {
                $("#sel_" + field + "_dst").append("<option value='" + list[i] + "'>" + list[i] + "</option>");
            } 
        }
    });

    // Carregando valores "default" dos campos ListToList
    $('.bw-ll-autoload').each(function() {
        var field, values, list, info;
        
        field = $(this).attr('name');
        values = $(this).attr('value');

        if (values != "") {
            list = values.split(",");

            for (i = 0; i < list.length; i++) {
                info = $("#sel_" + field + "_src option[value='" + list[i] + "']").text();
                $("#sel_" + field + "_src option[value='" + list[i] + "']").remove();
                $("#sel_" + field + "_dst").append("<option value='" + list[i] + "'>" + info + "</option>");
            }
        }
    });

    // Carregando valor "default" dos campos SelectList
    $(".bw-sl-autoload").each(function() {
        var field, value;
       
        field = $(this).attr('name');
        value = $(this).attr('value');

        $("#" + field + " option[value='" + value + 
          "']").prop("selected", true);
    });

    // Carregando valor "default" dos campos RadioList
    $(".bw-rl-autoload").each(function() {
        var field, value;
       
        field = $(this).attr('name');
        value = $(this).attr('value');

        $("form input:radio[name='" + field + "_opt'][value='" + 
          value + "']").prop("checked", true);

        if (!$("form input:radio[name='" + field + 
               "_opt']:checked").length > 0) {
            $("form input:radio[name='" + field + 
              "_opt']:first").prop("checked", true); 
        }
    });

    // Carregando valores "default" dos campos CheckBoxList
    $('.bw-cbl-autoload').each(function() {
        var field, values, list, info;

        field = $(this).attr('name');
        values = $(this).attr('value');

        if (values != "") {
            list = values.split(",");

            for (i = 0; i < list.length; i++) {
                $("form input:checkbox[name='" + field + "_opt'][value='" +
                   list[i] + "']").prop("checked", true);
            }
        }
    });
}

function updateList(fieldname) {
    var obj = $("#sel_" + fieldname + "_dst");
    var id_list = "";
    var list = new Array();

    if (obj.has('option').length > 0) {
        $("#sel_" + fieldname + "_dst > option").each(function() {
            list.push(this.value);
        });

        id_list = list.join(",");
    }

    $("#" + fieldname).val(id_list);
}

function updateCheckBoxList(fieldname) {
    var list = new Array();

    $("form input:checkbox[name='" + fieldname + "_opt']:checked").each(function() {
        list.push(this.value);
    });

    $("#" + fieldname).val(list.join(","));
}

function loadPaginator(href) {
    var nr_pages = $("#bw-table-list").attr("bw-nr-pages");
    var qs = href.split("&");

    var options = {
        bootstrapMajorVersion: 3,
        totalPages: nr_pages,
        size:'small',
        onPageClicked: function(e, originalEvent, type, page) {
            var cur_page = $("#bw-table-list").attr("bw-page");
            if (cur_page != page) {
                var pdata = "curpage=" + page;
                $.post(qs[0], pdata, function(data) {
                    $("#bwpanel").html(data);
                    $("#bwpaginator").bootstrapPaginator(options);
                    newopt = { currentPage: page };
                    $("#bwpaginator").bootstrapPaginator(newopt);
                });
            }
        }
    }

    $("#bwpaginator").bootstrapPaginator(options);
}

function reloadPanel(href, timeout) {
    var qs = href.split("&");
    var page = $("#bw-table-list").attr("bw-page");
    var pdata = "curpage=" + page;

    $.post(qs[0], pdata, function(data) {
        $("#bwpanel").html(data);
        loadPaginator(href);
        
        if (timeout > 0) {
            timeoutHandler = setTimeout(function() {
                reloadPanel(href, timeout);
            }, timeout * 1000);
        }
    });
}

