#include "plugin.h"

static void handle_parameters(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            return;
        }
        context->go_to_offset = false;
    }

    switch (context->next_param) {
        case TOKEN_ADDRESS:  // input/output token
            PRINTF("TOKEN_ADDRESS\n");
            copy_address(context->token_address, msg->parameter, sizeof(context->token_address));
            context->next_param = TOKEN_AMOUNT;
            break;
        case TOKEN_AMOUNT:  // amountToken/amountMToken
            PRINTF("TOKEN_AMOUNT\n");
            copy_parameter(context->token_amount, msg->parameter, sizeof(context->token_amount));
            context->next_param = UNSUPPORTED_PARAMETER;
            break;
        case UNSUPPORTED_PARAMETER:  // skip referrerId and minToReceive
            PRINTF("UNSUPPORTED_PARAMETER\n");
            break;
        // Keep this
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(ethPluginProvideParameter_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;

    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`) and then
    // the address (here `msg->parameter`).
    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case DEPOSIT_INSTANT:
        case REDEEM_INSTANT:
            handle_parameters(msg, context);
            break;
        case DEPOSIT_REQUEST:
        case REDEEM_REQUEST:
        case REDEEM_FIAT_REQUEST:
            handle_parameters(msg, context);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
